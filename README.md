# Design Decisions

## Latch Structs and Processor Class
- We made the program modular by modelling the IF-ID, ID-EX, EX-MEM and MEM-WB latches as structs. We have kept all the control signals as members of the respective structs.
- Other members of the latches include: Program Counter for each struct (to denote the instruction present in the respective stage), flag for last instruction (end of pipeline), NOP flag, and ON member to detect stalls.
- We created forwarded and non-forwarded processor classes and instantiated them with the four latch structs and other components of the processor like a memory-simulating array, register array and an instruction vector. The run function models the Central Processing Unit: calling the stage functions based on the hazard and supplying PC values based on jumps and branches. 

## Executing and Printing the Pipeline
- We have modelled the clock by executing the pipeline cycle by cycle. The output obtained from this algorithm is a vertical, skewed one and without dashes (-) in stalls. To convert this to the required form, we store the stage code in an answer matrix, which is printed at the end of the pipeline. For example, setting ans(i , j)=2 will print ID in the j th cycle of the i th instruction.
- At any cycle, we execute the pipeline stages in an inverted order i.e. the stage functions are called in the order WB->MEM->EX->ID->IF. This is based on the intuition that we are basically trying to model parallel hardware through sequential algorithms. To illustrate this, consider arbitrary members L1.x, L2.x and L3.x: if the ID stage is called before EX stage, we propagate the members by doing L2.x=L1.x and hence the data in L2.x is lost (can't be propagated to L3). Calling EX before ID preserves this data as L3.x is set to L2.x first and then L2.x is set to L1.x

## Memory and Register Layout
- We simulated the memory component by creating an arbitrary 1024-length vector. Each element in the vector models a byte in the memory.
- Correspondingly, we have implemented the load byte, load word and load double instructions by taking 1, 4 and 8 bytes starting at the computed memory address. The format followed is little endian. Same for store byte, store word and store double.
- Simulated the registers by creating a 32-size vector. We have hardwired x0 register to zero value i.e. write backs to x0 are ignored.

## Hazard Detection and Stalls
- All hazards have been detected in the ID stage through an integer return value. The number of stalls are not predetermined (this might be possible in hardware but requires an additional decrementing counter variable) i.e. in each cycle we go into the IDSTAGE function to check for a stall.
- Used ON members in latches to detect if a given latch is in use in the current cycle. A useful observation in our design is that if latch<sub>i</sub> is in use in cycle t<sub>j</sub> then latch<sub>i+1</sub> will definitely be in use in cycle t<sub>j+1</sub>. Similar result for L<sub>i</sub> not in use in current cycle. These observations help toggle the latches ON and OFF for the next cycle. 
- We have explicitly used a NOP member to flush out instructions in case of branch taken and jump. We converted such instructions to NOP in the CPU itself. When a stage is called, we simply return back to the CPU in case of NOP, after propagating that instruction across the stage. 

## Other Important Assumptions
- We have assumed that the results of EX, MEM and WB stages are available before the ID stage performs decode. This is consistent with the RIPES simulator and simplifies the implementation for both forwarded and non-forwarded cases.
- An additional case worth noting is when the same instruction is present in different stages in the same cycle. Suppose instructions are ordered as I1->I2->I1 (I2 jumps back to I1) then two instances of I1 will be present in the pipeline in the same cycle. RIPES handles this by printing both the stages in the I1 row separated by a slash ( / ). In our design, we have explicitly worked under the assumption that this is not possible, so we check in the IF stage: If the instruction being fetched in the current cycle is already present in some other stage, we stall the IF stage until the first instance of that instruction exits out of the pipeline.

*NOTE:* We have simulated all operations extensively and updated the register values at each stage of the computation. Moreover, we have not assumed that branch will always not be taken, but have explicitly checked the branch condition in the ID stage and taken/not taken the branch based on that. 



