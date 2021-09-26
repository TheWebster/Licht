# Licht
`licht` ( lɪçt. German for "light") is a commandline tool for changing the brightness of the backlight of laptop screens. It shares its main usecase with its English brother [light](https://github.com/haikarainen/light) but doesn't use any of its codebase. The main and only advantage of `licht` is that it provides smooth transition of the backlight, if the brightness control is fine grained enough.

---
### How it works
An invocation of `licht` starts a short lived daemon that lives as long as the transition lasts. Other instances, instead of writing blindly to the device, can then connect to that daemon and request the new brightness value. The daemon then stops the current transition and starts the new one.

There are five commands:

+ `get` : Print the current target value and exit.
  
  Note that it displays the *target* value. So if for example a daemon is currently transitioning towards 20% the output will be 20.00000.
+ `set` : Set the brightness to a value and return the same value.
  ```
  % licht set 20
  20.00000
  ```
+ `add` : Add to the current brightness value and return the new value.
  ```
  % licht add 7.5
  27.50000
  ```
+ `sub` : Subtract from the current brightness value and return the new value.
  ```
  % licht sub 2.5
  25.00000
  ```
+ `mul` : Multiply a value with th current brightness value and return the new value.
  ```
  % licht mul 0.5
  12.50000
  ```
---
### licht.conf
The config file is of the general form
```ini
KEY = VALUE
```
Whitespace before and after KEY and VALUE are ignored, as are blank lines and lines containing unknown keys. So it is possible to comment out lines by prefixing them with '#' or any other character.<br>
`licht` first reads the systemwide config file `/etc/Licht.conf`. Then, if UID is not 0, it reads the per user config file `$XDG_CONFIG_HOME/Licht.conf`. Entries in the per user file overwrite entries in the systemwide file. The following key value pairs are recognized:
+ `DEVICE = <path-to-device>` : Absolute path to a /sys directory (or any other kind of directory) that contains the files **brightness** and **max_brightness**. If this key is not present in both configuration files, it defaults to the first directory in `/sys/class/backlight`, which is usually all there is and is what the user wants to control.
+ `SMOOTH_DURTION = <milliseconds>` : The duration a transition to another brightness value will take. Defaults to 0, which means instantneous change.
+ `SMOOTH_INTERVAL = <milliseconds>` : The time between writes to the device. A sensible value would be 40ms, which results in 25 changes per second.
+ `RANGE_MAX = <value>` : Maximum brightness that can be set. An input of 100 will then be mapped to `<value>`.
+ `RANGE_MIN = <value>` : Minimum brightness that can be set. An input of 0 will then be mapped to `<value>`.
