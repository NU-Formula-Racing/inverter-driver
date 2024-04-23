# Inverter Driver
### Inverter driver with an abstract inverter class and a specific implementation for the Bamocar D3 inverter.  
<br>

## To use
#### Initialize the inverter driver on setup
```inverter.Initialize();```
#### Request readings and status from the inverter and provide the frequency of which you would like to receive the reading, ie motor temperature every 200ms. Note: this request message may not be received if it is only sent once, so it would be safer to request it at some frequency to ensure that the message gets through.
```inverter.RequestMotorTemperature(200);```
#### Send torque requests to the inverter requesting a percentage of the maximum torque
```inverter.RequestTorque(50);```

TODO:
- Implement status report mesages
- Forwarding received values onto another address
- Unit testing
- Additional safety checking