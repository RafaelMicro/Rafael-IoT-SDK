# Configuration System (Kconfig)
## Introduction
The Rafael IoT SDK's applications and kernel components can be tailored to meet specific application and platform requirements during the build process. This customization is facilitated through Kconfig, the same powerful configuration system used by the Linux kernel. The primary objective is to enable extensive configuration capabilities without the need to alter any source code directly, making the development process more efficient and flexible.

Configuration options, referred to as symbols, are defined within Kconfig files. These symbols can represent various settings and features that can be toggled on or off depending on the needs of the application. Kconfig files also outline dependencies between different symbols, ensuring that only valid configurations are applied. This prevents conflicts and ensures that all dependencies are properly managed.

Symbols can be grouped into menus and sub-menus, which helps in maintaining an organized and user-friendly configuration interface. This structured approach allows developers to easily navigate through different configuration options, making it straightforward to enable or disable features as required. By using Kconfig, developers can quickly adapt the Rafael IoT SDK to various hardware platforms and application requirements, ensuring optimal performance and functionality.

---
## Using Kconfig interfaces
Two interactive configuration interfaces are available for exploring Kconfig options and making temporary changes: menuconfig and guiconfig. menuconfig is a terminal-based interface that uses curses, while guiconfig provides a graphical interface for configuration.

> :grey_exclamation: To use guiconfig on WSL, you need to install Linux GUI applications. For more information, please refer to [this guide](https://learn.microsoft.com/zh-tw/windows/wsl/tutorials/gui-apps).

You can also change the configuration by manually editing the .config file in the project root directory. However, using one of the configuration interfaces is often more convenient, as they properly manage dependencies between configuration symbols.

If you attempt to enable a symbol with unsatisfied dependencies in ```.config```, the assignment will be ignored and overwritten when reconfiguring.

---
## KConfig Basic Using

The menuconfig and guiconfig interfaces provide versatile tools for configuring applications during development, allowing for experimentation with various settings. This tutorial aims to detail the process of making these configurations permanent and reliable, ensuring consistency and stability in your development environment. You can explore more about Kconfig language and its capabilities here. Understanding these interfaces enhances your ability to tailor applications precisely to your needs, promoting efficient and effective development practices.

The menuconfig and guiconfig interfaces can be used to experiment with configurations during application development. This guide explains how to make those settings permanent. (https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html)
- Basic Config: This keyword defines a new configuration option
    ``` kconfig
    config  <config_name>
        <config_type>  <prompt>
        default <default_value/default_section>
        <depends_section>
        <select_section>
        help
            <help_message>
    ```
    example
    ``` kconfig
    config FPU
        bool "Support floating point operations"
        depends on HAS_FPU
    ```
    In menuconfig, it looks like this:

    ``` kconfig
    [ ] Support floating point operations
    ```
- comment: Defines a comment displayed to the user during configuration, which is also written to the output file.
    ``` kconfig
    # This is comment
    ```
- menu/endmenu: Defines a menu where all options depending on it are its suboptions.
    ``` kconfig
    menu "MENU_A"
    endmenu
    ```
- choice/endchoice: Defines a group of choices.
    ``` kconfig
    choice
        prompt "EXAMPLE_CHOICE"
        default VAL_DEFAULT
    config VAL_DEFAULT
        int 0
    config VAL_1
        int 1
    config VAL_2
        int 2
    endchoice
    ```
- if/endif: Defines an if structure.
    ``` kconfig
    if VAL_1
    endif # VAL_1
    ```
- source: entry other Kconfig.
    ``` kconfig
    source "<path>/Kconfig"
    ```
- config dependence: add config dependence (B, C depend on A)
    ```
    config A
        bool "A"
        default 1
        select B
        select C

    config B
        bool "B"
        depends on A
        default 0

    config C
        bool "C"
        depends on A
        default 0
    ```

---
## KConfig in Rafal IoT SDK
The important configuration settings control building from following sources:
- Device configurations: config/devices.config</br>Manage CHIP_TYPE and BOARD_TYPE.
- Component configurations: config/components.config</br>Manage Components enable or not.
- Project configurations: config/example.config</br>Select example for building.

The detial component and project configuration value need setup at component or project Kconfig (ex:components/cpc/Kconfig)