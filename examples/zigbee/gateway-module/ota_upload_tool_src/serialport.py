import serial


class MySerial():
    def __init__(self, port, baudrate, log=print):
        self.port = port
        self.baudrate = baudrate
        self.log = log
        self.serPort = None
        self.serPortOpened = False

    def open(self):
        self.log("Open {}".format(self.port))
        if self.serPortOpened:
            return True
        try:
            self.serPort = serial.Serial()
            self.serPort.port     = self.port
            self.serPort.baudrate = self.baudrate
            self.serPort.timeout  = 0.05
            self.serPort.bytesize = serial.EIGHTBITS
            self.serPort.parity   = serial.PARITY_NONE
            self.serPort.stopbits = serial.STOPBITS_ONE
            self.serPort.rtscts   = False
            self.serPort.dsrdtr   = False

            self.serPort.setDTR(False)
            self.serPort.setRTS(False)

            self.serPort.open()

            if self.serPort.isOpen():
                self.serPortOpened = True
                self.log("%s opened" % self.serPort.port)
                return True
            self.serPort = None
            self.log("CANNOT OPEN COM PORT")
            return False

        except Exception as e:
            self.serPort = None
            self.log("CANNOT OPEN COM PORT: {}".format(e))
            return False

    def close(self):
        if self.serPortOpened:
            self.serPort.close()
            self.serPortOpened = False
            self.log("Port Closed {}".format(self.port))
            self.serPort = None

    def send_hex_serial(self, command: list[int]):
        self.serPort.write(bytes(command))
