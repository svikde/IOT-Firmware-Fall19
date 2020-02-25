Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

**1. How much current does a single LED draw when the output drive is set to "Strong" with the original code?**

Avg Current for On cycle of LED1: 4.85 mA

Avg Current for Off cycle of LED1: 4.36 mA

Current Consumed by LED1 = 4.85 mA - 4.36 mA = 0.49 mA

**2. After commenting out the standard output drive and uncommenting "Weak" drive, how much current does a single LED draw?**

Avg Current for On cycle of LED1: 4.83 mA

Avg Current for Off cycle of LED1: 4.35 mA

Current Consumed by LED1 = 4.85 mA - 4.36 mA = 0.48 mA

**3. Is there a meaningful difference in current between the answers for question 1 and 2? Please explain your answer, 
referencing the [Mainboard Schematic](https://www.silabs.com/documents/public/schematic-files/WSTK-Main-BRD4001A-A01-schematic.pdf) and [AEM Accuracy](https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf) section of the user's guide where appropriate.**

A strong drive strenght provides a 10mA drive current while a weak one provides a 1mA drive current. The contrast between both the currents from answers 1 and 2
are not prominent in this case even under differnt drive strengths. This is because they are not driving a significant load. The led consumes very less current
and hence a constant voltage and resistance yeilds almost similar current in both cases (ohms law). A significant difference probably would have been noticed if 
we had loads that drew more current. As per Ohms Law, if we need more current there must be a way to decrease the current limiting resistance with the LED or increase
the applied voltage.

**4. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with only LED1 turning on in the main loop?**

Current: 4.51 mA, Energy: 11.33 uWh 

**5. Using the Energy Profiler with "weak" drive LEDs, what is the average current and energy measured with both LED1 and LED0 turning on in the main loop?**

Current: 4.84 mA, Energy: 12.19 uWh
