# Building Open Tag Reader
## Bill of Materials
|Component|Qty| Source |
|---------|:---:|--------|
|Tag reader board|1|[FDX-B/HDX RFID Reader Writer with TTL serial port](https://www.priority1design.com.au/shopfront/index.php?main_page=product_info&cPath=1&products_id=10)|
Right angle header|1| Optional, or solder wires direct to board |
|Reader/Antenna Carrier|1|[3D Print - Coil Reader](/3d_print_files/)|
|Shroud|1|[3D Print - Coil Reader Shroud](/3d_print_files/)|
|LilyGoT-Display Plus|1|[T-Display Plus](https://lilygo.cc/products/t-display-s3-amoled-plus)|
|Connecting wire|1|Ribbon cable and Dupont connectors|
|LiPo Battery|1| |
|Boost converter|1| |
|Switch|1|
|Battery Holder|1|[3D Print](/3d_print_files/)|
|Screen adapter|1|[3D Print](/3d_print_files/)|
|End cap|1|[3D Print](/3d_print_files/)|

## Prepare Reader Board
1. Solder right angle header.


## Attach antenna
1. Seat antenna in groove aligning pigtails with the slots.
2. Feed under and through board at A+ and A-
3. Check wires seated and fix in place.
4. Solder antenna to board.

## Flash Firmware

## Prepare Pipe
1. Drill holes for Shroud/carrier board
2. Make slot for display.
3. Drill holes for end cap

3D Printed jigs available for drilling holes and marking cuts


## Wiring
### Simple Wiring (Direct to board)
No PCB needed but probably isn't going to be super reliable.

Clip off JST connect from battery (be careful about shorts).  Connect the red +ve wire to to the red wire for the small connector that comes with the display unit.  The black -ve wires to either side of a switch.

### Fancy Carrier Board
This includes headers to plug things into (theorectically more robust),mounts the power supply for the reader board, and includes a buzzer, and circuitry to control power to the reader board and drive a led on the ON/OFF switch.

### Minimalist
Powered by USB!, no battery, no switch, just 3 wires!