# Usage

## Wiring

The TX pin of one device must be connected to the RX of another. For boards with more than one serial port assure that the wires match up with the correct TX and RX pins of the port chosen [here](#begin).

## Include

To use classes and functions in the library, you must first include the correct header file into your `*.ino` file.
```c++
#include <ArgDevice.hpp>
```

## Setup

~~Examples of code can be found in [UARGs/examples]()~~ (not in version 1.0.0).

### Creation

This library uses `ArgDevice` objects to read and send messages. There are two different ways to create an `ArgDevice`:

-
    ```c++
    ArgDevice device();
    ```

    This is the primary way that should be used most of the time.

- 
    ```c++
    ArgDevice device(true);
    ```

    ~~This will enable robust logging to your serial monitor. This may affect the available buffer size on your serial port momentarily after an item is logged, and so it should only really be used for debugging purposes.~~  
    This should not be used in version 1.0.0 as the logging will never be read, making the available buffer size always 0. This should be changed in version 1.0.1.

### Begin

**Once the device has been begun, do not begin any of the serial ports in use. This may change the baud rate of the port and make the data unusable.**

The `ArgDevice` must also be begun in the setup function. There are multiple ways to do this:

-
    ```c++
    device.begin();
    ```

    This will begin the device using your board's default serial port.

-
    ```c++
    device.begin(&(_your serial port_));
    ```

    This will begin the device using the port you choose. Replace '_your serial port_' with something like `Serial2` or `Serial5`.

-
    ```c++
    device.begin(&(_your sending port_), &(_your receiving port_));
    ```

    This will begin the device using specific sending and receiving ports you choose. Replace '_your sending port_' and '_your receiving port_' with something like `Serial2` or `Serial5`.

## Communication

Once the device has begun, you can get to communicating between other begun devices on different boards.

### Sending

Sending messages are much more simple than recieving.

```c++
device.sendArgs("message");
```
Your message must be be in format: `command arg arg` with spaces seperating each argument. For example: `add 5 2 4 9`.

### Receiving

There are multiple ways to recieve a message, one providing more control over the other. The most basic way is using the command listener system.

#### Command Listener

Before adding a listener to a command, you must make sure you include `device.update();` in your code. This will read any pending messages and call the callback for any pending messages.

```c++
void loop() {
    device.update();
}
```
This does not need to be called in a loop, but no callbacks will be run until `update` is used.

##### Adding

To add a listener for a command, use:
```c++
device.addEventListener("commandToListenFor", callback);

void callback(Args args) {

}
```
The function `callback` will be called whenever the command `commandToListenFor` is found in a message. **The function `callback` must return void and take one parameter, Args args.**   There can be more than one listener for one command.

The args object contains the arguments and number of arguments that was sent with the command.

To obtain the arguments, use:
```c++
Arg arg = device.getArg(&args, index);
```
where index is the number of the argument (using 0 based indexing).

If an invalid index is used an `Arg` with value `"NULLARG"` will be returned.

To get the string value of an arg, use:
```c++
String arg = arg.v;
```

##### Removing

To remove a listener, use:
```c++
device.removeEventListener("commandToListenFor", callback);
```
Ensure that both the command to listen for and the callback are the same as when you added it.

#### Alternative system

To check and get if there are any pending messages, you use:

```c++
Args args = device.receiveArgs();
```

You can then check if there was a pending message by making sure there are more than 0 arguments. Even if there were no arguments intended to be send, there will still technically be 1 specifically for this reason.

```c++
if(args.amount > 0) {

}
````

Once a message is received, the data can be gotten from the `Args` object.

-
    To get the command:
    ```c++
    String command = args.command
    ```

-
    To get an argument:
    ```c++
    String argument = device.getArg(&args, index).v;
    ```
    `index` is the index of argument you are getting. Uses 0 based indexing. `.v` **is required to get the string value of the arg, otherwise it is a struct.**