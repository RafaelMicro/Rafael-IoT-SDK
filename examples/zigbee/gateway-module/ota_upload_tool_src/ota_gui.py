"""GUI for building a Zigbee OTA image and uploading it to the gateway-module.

Combines create_image.py and ota_upload.py so start.bat and config.ini no
longer need to be edited:

    python ota_gui.py

Requires pyserial. Settings of the last run are kept in ota_gui.ini next to
this script.
"""

import configparser
import queue
import sys
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

import serial.tools.list_ports

import create_image
import rafael_image
from ota_upload import OtaUploader

if getattr(sys, "frozen", False):          # PyInstaller exe
    APP_DIR = Path(sys.executable).parent
else:
    APP_DIR = Path(__file__).resolve().parent
SETTINGS_FILE = APP_DIR / "ota_gui.ini"

BAUDRATES = ("115200", "230400", "460800", "921600", "57600", "38400", "19200", "9600")

DEFAULTS = {
    "mode": "bin",
    "src_file": "",
    "out_file": "",
    "version": "v01.00.00.00",
    "manuf_code": "0x007B",
    "image_type": "0x0141",
    "sign": "0",
    "private_key": "",
    "public_key": "",
    "port": "",
    "baudrate": "115200",
    "sign_src_file": "",
    "sign_out_file": "",
}


class OtaGui:
    def __init__(self, root):
        self.root = root
        self.root.title("Zigbee OTA Upload Tool")
        self.root.minsize(640, 520)

        self.msg_queue = queue.Queue()   # (kind, value) from the worker thread
        self.worker = None
        self.uploader = None

        s = self._load_settings()
        self.mode = tk.StringVar(value=s["mode"])
        self.src_file = tk.StringVar(value=s["src_file"])
        self.out_file = tk.StringVar(value=s["out_file"])
        self.version = tk.StringVar(value=s["version"])
        self.manuf_code = tk.StringVar(value=s["manuf_code"])
        self.image_type = tk.StringVar(value=s["image_type"])
        self.sign = tk.BooleanVar(value=s["sign"] == "1")
        self.private_key = tk.StringVar(value=s["private_key"])
        self.public_key = tk.StringVar(value=s["public_key"])
        self.port = tk.StringVar(value=s["port"])
        self.baudrate = tk.StringVar(value=s["baudrate"])
        self.sign_src_file = tk.StringVar(value=s["sign_src_file"])
        self.sign_out_file = tk.StringVar(value=s["sign_out_file"])

        self._build_ui()
        self.refresh_ports()
        self._on_mode_change()
        self.root.protocol("WM_DELETE_WINDOW", self._on_close)
        self.root.after(100, self._poll_queue)

    # ------------------------------------------------------------------ UI
    def _build_ui(self):
        pad = {"padx": 6, "pady": 3}
        main = ttk.Frame(self.root, padding=8)
        main.pack(fill="both", expand=True)
        main.columnconfigure(0, weight=1)
        main.rowconfigure(1, weight=1)

        tabs = ttk.Notebook(main)
        tabs.grid(row=0, column=0, sticky="ew", **pad)
        ota_tab = ttk.Frame(tabs, padding=4)
        ota_tab.columnconfigure(0, weight=1)
        tabs.add(ota_tab, text="OTA build / upload")
        sign_tab = ttk.Frame(tabs, padding=4)
        sign_tab.columnconfigure(0, weight=1)
        tabs.add(sign_tab, text="Sign image")
        self._build_sign_tab(sign_tab, pad)

        # --- image source ---
        src = ttk.LabelFrame(ota_tab, text="OTA image")
        src.grid(row=0, column=0, sticky="ew", **pad)
        src.columnconfigure(1, weight=1)

        modes = ttk.Frame(src)
        modes.grid(row=0, column=0, columnspan=3, sticky="w", **pad)
        ttk.Radiobutton(modes, text="Build from firmware .bin", value="bin",
                        variable=self.mode, command=self._on_mode_change).pack(side="left")
        ttk.Radiobutton(modes, text="Use existing .ota file", value="ota",
                        variable=self.mode, command=self._on_mode_change).pack(side="left", padx=12)

        self.src_label = ttk.Label(src, text="Firmware:")
        self.src_label.grid(row=1, column=0, sticky="w", **pad)
        ttk.Entry(src, textvariable=self.src_file).grid(row=1, column=1, sticky="ew", **pad)
        ttk.Button(src, text="Browse...", command=self._browse_src).grid(row=1, column=2, **pad)

        self.out_label = ttk.Label(src, text="Save OTA as:")
        self.out_label.grid(row=2, column=0, sticky="w", **pad)
        self.out_entry = ttk.Entry(src, textvariable=self.out_file)
        self.out_entry.grid(row=2, column=1, sticky="ew", **pad)
        self.out_btn = ttk.Button(src, text="Browse...", command=self._browse_out)
        self.out_btn.grid(row=2, column=2, **pad)

        params = ttk.Frame(src)
        params.grid(row=3, column=0, columnspan=3, sticky="w", **pad)
        ttk.Label(params, text="File version:").pack(side="left")
        self.version_entry = ttk.Entry(params, textvariable=self.version, width=14)
        self.version_entry.pack(side="left", padx=(4, 14))
        ttk.Label(params, text="Manufacturer code:").pack(side="left")
        self.manuf_entry = ttk.Entry(params, textvariable=self.manuf_code, width=8)
        self.manuf_entry.pack(side="left", padx=(4, 14))
        ttk.Label(params, text="Image type:").pack(side="left")
        self.type_entry = ttk.Entry(params, textvariable=self.image_type, width=8)
        self.type_entry.pack(side="left", padx=4)
        ttk.Label(src, text="Version format vAA.BB.CC.DD (hex), codes 0x01AB",
                  foreground="gray").grid(row=4, column=0, columnspan=3, sticky="w", padx=6)

        # compression is fixed on, only signing is selectable
        opts = ttk.Frame(src)
        opts.grid(row=5, column=0, columnspan=3, sticky="w", **pad)
        ttk.Label(opts, text="Compression: LZMA (always on)").pack(side="left")
        self.sign_check = ttk.Checkbutton(opts, text="Sign image (ECDSA P-256)",
                                          variable=self.sign, command=self._on_mode_change)
        self.sign_check.pack(side="left", padx=20)

        self.key_widgets = []
        for row, (label, var, browse) in enumerate((
                ("Private key:", self.private_key, self._browse_private_key),
                ("Public key:", self.public_key, self._browse_public_key)), start=6):
            ttk.Label(src, text=label).grid(row=row, column=0, sticky="w", **pad)
            entry = ttk.Entry(src, textvariable=var)
            entry.grid(row=row, column=1, sticky="ew", **pad)
            btn = ttk.Button(src, text="Browse...", command=browse)
            btn.grid(row=row, column=2, **pad)
            self.key_widgets += [entry, btn]
        ttk.Label(src, text="Public key is optional: when given, the signature is verified "
                            "right after signing", foreground="gray").grid(
            row=8, column=0, columnspan=3, sticky="w", padx=6)

        # --- serial ---
        ser = ttk.LabelFrame(ota_tab, text="Gateway serial port")
        ser.grid(row=1, column=0, sticky="ew", **pad)
        ttk.Label(ser, text="Port:").pack(side="left", padx=6, pady=6)
        self.port_box = ttk.Combobox(ser, textvariable=self.port, width=40)
        self.port_box.pack(side="left", pady=6)
        ttk.Button(ser, text="Refresh", command=self.refresh_ports).pack(side="left", padx=6)
        ttk.Label(ser, text="Baudrate:").pack(side="left", padx=(12, 4))
        ttk.Combobox(ser, textvariable=self.baudrate, values=BAUDRATES,
                     width=9).pack(side="left", pady=6)

        # --- actions ---
        act = ttk.Frame(ota_tab)
        act.grid(row=2, column=0, sticky="ew", **pad)
        act.columnconfigure(3, weight=1)
        self.build_btn = ttk.Button(act, text="Create OTA only", command=self.on_create)
        self.build_btn.grid(row=0, column=0, padx=(0, 6))
        self.upload_btn = ttk.Button(act, text="Create && Upload", command=self.on_upload)
        self.upload_btn.grid(row=0, column=1, padx=(0, 6))
        self.cancel_btn = ttk.Button(act, text="Cancel", command=self.on_cancel, state="disabled")
        self.cancel_btn.grid(row=0, column=2, padx=(0, 6))
        self.progress = ttk.Progressbar(act, mode="determinate")
        self.progress.grid(row=0, column=3, sticky="ew", padx=6)
        self.progress_text = ttk.Label(act, text="", width=16)
        self.progress_text.grid(row=0, column=4)

        # --- log ---
        logf = ttk.LabelFrame(main, text="Log")
        logf.grid(row=1, column=0, sticky="nsew", **pad)
        logf.rowconfigure(0, weight=1)
        logf.columnconfigure(0, weight=1)
        self.log_text = tk.Text(logf, height=14, state="disabled", wrap="word")
        self.log_text.grid(row=0, column=0, sticky="nsew")
        sb = ttk.Scrollbar(logf, command=self.log_text.yview)
        sb.grid(row=0, column=1, sticky="ns")
        self.log_text["yscrollcommand"] = sb.set
        ttk.Button(logf, text="Clear log", command=self._clear_log).grid(
            row=1, column=0, columnspan=2, sticky="e", pady=2)

    def _build_sign_tab(self, tab, pad):
        # keys are shared with the OTA tab
        frame = ttk.LabelFrame(tab, text="Sign firmware image (ECDSA P-256, no compression)")
        frame.grid(row=0, column=0, sticky="ew", **pad)
        frame.columnconfigure(1, weight=1)
        for row, (label, var, browse) in enumerate((
                ("Input image:", self.sign_src_file, self._browse_sign_src),
                ("Save signed as:", self.sign_out_file, self._browse_sign_out),
                ("Private key:", self.private_key, self._browse_private_key),
                ("Public key:", self.public_key, self._browse_public_key))):
            ttk.Label(frame, text=label).grid(row=row, column=0, sticky="w", **pad)
            ttk.Entry(frame, textvariable=var).grid(row=row, column=1, sticky="ew", **pad)
            ttk.Button(frame, text="Browse...", command=browse).grid(row=row, column=2, **pad)
        ttk.Label(frame, text="Public key is optional: when given, the signature is verified "
                              "right after signing", foreground="gray").grid(
            row=4, column=0, columnspan=3, sticky="w", padx=6)
        ttk.Button(tab, text="Sign image", command=self.on_sign).grid(
            row=1, column=0, sticky="w", **pad)

    def _browse_sign_src(self):
        cur = self.sign_src_file.get()
        initial = str(Path(cur).parent) if cur and Path(cur).parent.exists() else self._initial_dir()
        path = filedialog.askopenfilename(
            filetypes=[("Firmware binary", "*.bin"), ("All files", "*.*")], initialdir=initial)
        if path:
            self.sign_src_file.set(path)
            p = Path(path)
            self.sign_out_file.set(str(p.with_name(p.stem + "_signed" + p.suffix)))

    def _browse_sign_out(self):
        cur = self.sign_out_file.get() or self.sign_src_file.get()
        initial = str(Path(cur).parent) if cur and Path(cur).parent.exists() else self._initial_dir()
        path = filedialog.asksaveasfilename(
            defaultextension=".bin", filetypes=[("Firmware binary", "*.bin"), ("All files", "*.*")],
            initialdir=initial, initialfile=Path(self.sign_out_file.get()).name)
        if path:
            self.sign_out_file.set(path)

    def _on_mode_change(self):
        is_bin = self.mode.get() == "bin"
        self.src_label["text"] = "Firmware:" if is_bin else "OTA file:"
        state = "normal" if is_bin else "disabled"
        for w in (self.out_entry, self.out_btn, self.version_entry,
                  self.manuf_entry, self.type_entry, self.build_btn, self.sign_check):
            w["state"] = state
        key_state = "normal" if is_bin and self.sign.get() else "disabled"
        for w in self.key_widgets:
            w["state"] = key_state
        self.upload_btn["text"] = "Create && Upload" if is_bin else "Upload"
        if not is_bin and self.src_file.get():
            self._fill_from_ota(self.src_file.get(), quiet=True)

    def _browse_src(self):
        if self.mode.get() == "bin":
            types = [("Firmware binary", "*.bin"), ("All files", "*.*")]
        else:
            types = [("Zigbee OTA image", "*.ota"), ("All files", "*.*")]
        path = filedialog.askopenfilename(filetypes=types, initialdir=self._initial_dir())
        if not path:
            return
        self.src_file.set(path)
        if self.mode.get() == "bin":
            self.out_file.set(str(Path(path).with_suffix(".ota")))
        else:
            self._fill_from_ota(path)

    def _browse_out(self):
        path = filedialog.asksaveasfilename(
            defaultextension=".ota", filetypes=[("Zigbee OTA image", "*.ota")],
            initialdir=self._initial_dir(), initialfile=Path(self.out_file.get()).name)
        if path:
            self.out_file.set(path)

    def _browse_key(self, var, title):
        cur = var.get()
        initial = str(Path(cur).parent) if cur and Path(cur).parent.exists() else self._initial_dir()
        path = filedialog.askopenfilename(
            title=title, initialdir=initial,
            filetypes=[("PEM key", "*.pem"), ("All files", "*.*")])
        if path:
            var.set(path)

    def _browse_private_key(self):
        self._browse_key(self.private_key, "Select ECDSA P-256 private key")

    def _browse_public_key(self):
        self._browse_key(self.public_key, "Select ECDSA P-256 public key")

    def _initial_dir(self):
        cur = self.src_file.get()
        return str(Path(cur).parent) if cur and Path(cur).parent.exists() else str(APP_DIR)

    def _fill_from_ota(self, path, quiet=False):
        try:
            image_type, manuf, version = create_image.parse_ota_header(Path(path).read_bytes())
        except (OSError, ValueError) as e:
            if not quiet:
                messagebox.showerror("Invalid OTA file", str(e))
            return None
        self.image_type.set("0x%04X" % image_type)
        self.manuf_code.set("0x%04X" % manuf)
        self.version.set(create_image.format_version(version))
        return image_type, manuf, version

    def refresh_ports(self):
        ports = sorted(serial.tools.list_ports.comports(), key=lambda p: p.device)
        self.port_box["values"] = [f"{p.device} - {p.description}" for p in ports]
        current = self._port_name()
        if not current and ports:
            self.port_box.current(0)
        elif current:
            for i, p in enumerate(ports):
                if p.device == current:
                    self.port_box.current(i)
                    break

    def _port_name(self):
        return self.port.get().split(" - ")[0].strip()

    # ------------------------------------------------------------------ log
    def log(self, text):
        """Thread safe: may be called from the worker."""
        self.msg_queue.put(("log", text))

    def _append_log(self, text):
        self.log_text["state"] = "normal"
        self.log_text.insert("end", text + "\n")
        self.log_text.see("end")
        self.log_text["state"] = "disabled"

    def _clear_log(self):
        self.log_text["state"] = "normal"
        self.log_text.delete("1.0", "end")
        self.log_text["state"] = "disabled"

    def _poll_queue(self):
        try:
            while True:
                kind, value = self.msg_queue.get_nowait()
                if kind == "log":
                    self._append_log(value)
                elif kind == "progress":
                    done, total = value
                    self.progress["maximum"] = total
                    self.progress["value"] = done
                    self.progress_text["text"] = f"{done}/{total} ({done * 100 // total}%)"
                elif kind == "done":
                    self._on_finished(value)
        except queue.Empty:
            pass
        self.root.after(50, self._poll_queue)

    # ------------------------------------------------------------------ actions
    def _build_image(self):
        """Create the OTA image from the inputs. Returns (bytes, type, manuf, version)
        or None after showing an error."""
        src = self.src_file.get().strip()
        if not src:
            messagebox.showerror("Missing input", "Please select a firmware/OTA file.")
            return None

        if self.mode.get() == "ota":
            try:
                image = Path(src).read_bytes()
                image_type, manuf, version = create_image.parse_ota_header(image)
                info = create_image.parse_rafael_info(image)
            except (OSError, ValueError) as e:
                messagebox.showerror("Invalid OTA file", str(e))
                return None
            if info is None or not info["compressed"]:
                messagebox.showerror(
                    "Invalid OTA file",
                    "This OTA file has no Rafael image header or is not compressed.\n"
                    "It was probably made by an older tool. Build it again from the "
                    "firmware .bin.")
                return None
            self.log(f"Loaded {src}")
            self.log(f"  version 0x{version:08X}, manufacturer 0x{manuf:04X}, image type 0x{image_type:04X}")
            self.log(f"  compressed: yes, signed: {'yes' if info['signed'] else 'no'}, "
                     f"image length {info['image_len']}")
            return image, image_type, manuf, version

        try:
            version = create_image.parse_version(self.version.get().strip())
            manuf = create_image.parse_hex16(self.manuf_code.get().strip(), "manufacturer code")
            image_type = create_image.parse_hex16(self.image_type.get().strip(), "image type")
        except ValueError as e:
            messagebox.showerror("Invalid parameter", str(e))
            return None
        try:
            firmware = Path(src).read_bytes()
        except OSError as e:
            messagebox.showerror("Cannot read firmware", str(e))
            return None

        private_key = public_key = None
        sign = self.sign.get()
        try:
            if sign:
                if not self.private_key.get().strip():
                    messagebox.showerror("Missing input", "Please select the private key used for signing.")
                    return None
                private_key = rafael_image.load_private_key(self.private_key.get().strip())
                if self.public_key.get().strip():
                    public_key = rafael_image.load_public_key(self.public_key.get().strip())
            self.log(f"Building OTA image from {src}")
            self.log(f"  version 0x{version:08X}, manufacturer 0x{manuf:04X}, image type 0x{image_type:04X}")
            self.log(f"  compression: LZMA, signing: {'enabled' if sign else 'disabled'}")
            image = create_image.create_ota_image(firmware, version, manuf, image_type,
                                                  private_key, public_key, log=self.log)
        except rafael_image.RafaelImageError as e:
            messagebox.showerror("Cannot build OTA image", str(e))
            return None
        self.log(f"  size {len(image)} bytes, AES-MMO hash "
                 + " ".join("%02X" % b for b in image[-create_image.OTA_HASH_LENGTH:]))

        out = self.out_file.get().strip()
        if out:
            try:
                Path(out).write_bytes(image)
                self.log(f"  written to {out}")
            except OSError as e:
                messagebox.showerror("Cannot write OTA file", str(e))
                return None
        return image, image_type, manuf, version

    def on_create(self):
        if self._build_image():
            self._save_settings()

    def on_sign(self):
        src = self.sign_src_file.get().strip()
        out = self.sign_out_file.get().strip()
        if not src or not out:
            messagebox.showerror("Missing input", "Please select the input image and the output file.")
            return
        if not self.private_key.get().strip():
            messagebox.showerror("Missing input", "Please select the private key used for signing.")
            return
        try:
            firmware = Path(src).read_bytes()
        except OSError as e:
            messagebox.showerror("Cannot read image", str(e))
            return
        try:
            private_key = rafael_image.load_private_key(self.private_key.get().strip())
            public_key = (rafael_image.load_public_key(self.public_key.get().strip())
                          if self.public_key.get().strip() else None)
            self.log(f"Signing {src}")
            signed = rafael_image.sign_firmware(firmware, private_key, public_key, log=self.log)
        except rafael_image.RafaelImageError as e:
            messagebox.showerror("Cannot sign image", str(e))
            return
        try:
            Path(out).write_bytes(signed)
        except OSError as e:
            messagebox.showerror("Cannot write signed image", str(e))
            return
        self.log(f"  written to {out}")
        self._save_settings()
        messagebox.showinfo("Sign image", f"Signed image written to\n{out}")

    def on_upload(self):
        port = self._port_name()
        if not port:
            messagebox.showerror("Missing input", "Please select the gateway serial port.")
            return
        try:
            baud = int(self.baudrate.get())
        except ValueError:
            messagebox.showerror("Invalid parameter", "Baudrate must be a number.")
            return

        built = self._build_image()
        if not built:
            return
        image, image_type, manuf, version = built
        self._save_settings()

        self.uploader = OtaUploader(port, baud, image, image_type, manuf, version,
                                    log=self.log,
                                    progress=lambda d, t: self.msg_queue.put(("progress", (d, t))))
        self.msg_queue.put(("progress", (0, self.uploader.total_block)))
        self._set_busy(True)
        self.worker = threading.Thread(target=self._upload_worker, daemon=True)
        self.worker.start()

    def _upload_worker(self):
        ok = self.uploader.run()
        self.msg_queue.put(("done", ok))

    def on_cancel(self):
        if self.uploader:
            self.log("Cancelling...")
            self.uploader.cancel()
            self.cancel_btn["state"] = "disabled"

    def _on_finished(self, ok):
        self._set_busy(False)
        self.uploader = None
        if ok:
            messagebox.showinfo("OTA upload", "OTA image uploaded and set as candidate.")
        else:
            messagebox.showerror("OTA upload", "Upload failed, see the log for details.")

    def _set_busy(self, busy):
        self.upload_btn["state"] = "disabled" if busy else "normal"
        self.cancel_btn["state"] = "normal" if busy else "disabled"
        self.build_btn["state"] = "disabled" if busy or self.mode.get() != "bin" else "normal"

    def _on_close(self):
        if self.worker and self.worker.is_alive():
            if not messagebox.askyesno("Upload in progress", "An upload is running. Abort and quit?"):
                return
            self.uploader.cancel()
            self.worker.join(timeout=2)
        self._save_settings()
        self.root.destroy()

    # ------------------------------------------------------------------ settings
    def _load_settings(self):
        s = dict(DEFAULTS)
        # first run: pick up port/baudrate from the command line tool's config.ini
        legacy = configparser.ConfigParser()
        if legacy.read(APP_DIR / "config.ini") and legacy.has_section("Port Config"):
            s["port"] = legacy["Port Config"].get("port", s["port"])
            s["baudrate"] = legacy["Port Config"].get("baudrate", s["baudrate"])
        cfg = configparser.ConfigParser()
        if cfg.read(SETTINGS_FILE) and cfg.has_section("gui"):
            for key in s:
                s[key] = cfg["gui"].get(key, s[key])
        return s

    def _save_settings(self):
        cfg = configparser.ConfigParser()
        cfg["gui"] = {
            "mode": self.mode.get(),
            "src_file": self.src_file.get(),
            "out_file": self.out_file.get(),
            "version": self.version.get(),
            "manuf_code": self.manuf_code.get(),
            "image_type": self.image_type.get(),
            "sign": "1" if self.sign.get() else "0",
            "private_key": self.private_key.get(),
            "public_key": self.public_key.get(),
            "port": self._port_name(),
            "baudrate": self.baudrate.get(),
            "sign_src_file": self.sign_src_file.get(),
            "sign_out_file": self.sign_out_file.get(),
        }
        try:
            with open(SETTINGS_FILE, "w") as f:
                cfg.write(f)
        except OSError:
            pass


def main():
    root = tk.Tk()
    OtaGui(root)
    root.mainloop()


if __name__ == "__main__":
    main()
