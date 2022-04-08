# safe

Safe created with arduino nano. Password is stored in internal arduino eeprom. Password and options can be selected by 4x4 keypad. Options and informations are displayed on 16x2 lcd display connected to arduino with i2c bus. Internal switch checks if the safe doors are closed and if they are closed prevents the arduino of being flashed. Safe also has buzzer which give feedback to user and play alarm at the wrong password.

Libraries used:
liquid crystal i2c - https://github.com/johnrickman/LiquidCrystal_I2C