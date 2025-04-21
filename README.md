# RL78_F24_PWM_4CH
 RL78_F24_PWM_4CH

udpate @ 2025/04/21

1. initial F24 EVB , to test PWM channel 

- UART : P15,P16 , to printf message

- PWM : P12 : TO11 , P11 : TO12 , P10 : TO13 , P31 : TO14

2. base on PWM master value , to calculate target slave channel duty 

ex : 

master channel TDR10 : 0xF9F

if slave channel 1 plan to set duty 100% , TDR11 will be (TDR10+1) * (4/4)

if slave channel 2 plan to set duty 75% , TDR11 will be (TDR10+1) * (3/4)

if slave channel 3 plan to set duty 50% , TDR11 will be (TDR10+1) * (2/4)

if slave channel 4 plan to set duty 25% , TDR11 will be (TDR10+1) * (1/4)

refer to 

```c
PWM_Channel_Duty_Increase
PWM_Channel_Duty_Decrease
```

3. use terminal to control PWM channel duty

press digit 1 ~ 4 , to switch PWM channel

press digit A/a , to increase the target PWM channel DUTY ( ex :+5%)

press digit D/d , to decrease the target PWM channel DUTY ( ex :-5%)

4. below is the log message

if set CH1:45% , CH2:10% , CH3:60% , CH4:80%

![image](https://github.com/released/RL78_F24_PWM_4CH/blob/main/log1.jpg)

![image](https://github.com/released/RL78_F24_PWM_4CH/blob/main/log2.jpg)

![image](https://github.com/released/RL78_F24_PWM_4CH/blob/main/LA1.jpg)


if set CH1:50% , CH2:50% , CH3:50% , CH4:50%

![image](https://github.com/released/RL78_F24_PWM_4CH/blob/main/log3.jpg)

![image](https://github.com/released/RL78_F24_PWM_4CH/blob/main/LA2.jpg)

