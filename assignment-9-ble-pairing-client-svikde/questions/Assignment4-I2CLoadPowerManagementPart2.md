Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements in the "Default" configuration of the profiler to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See [Shared document](https://docs.google.com/document/d/1Ro9G2Nsr_ZXDhBYJ6YyF9CPivb--6UjhHRmVhDGySag/edit?usp=sharing) for instructions.* 

1. What is the average current per period?
   Answer: 4.19 uA
   <br>Screenshot:  
   ![Avg_current_per_period](https://github.com/CU-ECEN-5823/assignment-4-svikde/blob/master/screenshots/Assignment4/Avg_current_per_period.JPG)  

2. What is the average current when the Si7021 is Load Power Management OFF?
   Answer: 1.83 uA
   <br>Screenshot:  
   ![Avg_current_LPM_Off](https://github.com/CU-ECEN-5823/assignment-4-svikde/blob/master/screenshots/Assignment4/Avg_current_LPM_Off.JPG)  

3. What is the average current when the Si7021 is Load Power Management ON?
   Answer: 97.67 uA
   <br>Screenshot:  
   ![Avg_current_LPM_On](https://github.com/CU-ECEN-5823/assignment-4-svikde/blob/master/screenshots/Assignment4/Avg_current_LPM_On.JPG)  

4. How long is the Si7021 Load Power Management ON for 1 temperature reading?
   Answer: 91.20 mS
   <br>Screenshot:  
   ![duration_lpm_on](https://github.com/CU-ECEN-5823/assignment-4-svikde/blob/master/screenshots/Assignment4/duration_lpm_on.JPG)  

5. What is the total operating time of your design for assignment 4 in hours assuming a 1000mAh supply?

1000 mAh = Avg_current_per_period * Time
hence, Time = 1000 mAh / 4.19uA = 238663.48 hrs

6. How has the power consumption performance of your design changed since the previous assignment?

It has reduced from what was observed in the 3rd assignment.

7. Describe how you have tested your code to ensure you are sleeping in EM1 mode during I2C transfers.

The Sleep Block Begin and Sleep Block End at the begining and end of I2C Transfers ensure that the system is Sleeping in EM1.
Moreover, on debugging the code and parsing the return value of SLEEP_sleep(), we can see that it returns sleepEM1. The current
profiler also shows current levels commensurate to what would be expected for EM1 during the I2C Transfers.