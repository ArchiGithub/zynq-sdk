proc add_address_module {module_name bram_width clk offset} {

  set bd [current_bd_instance .]
  current_bd_instance [create_bd_cell -type hier $module_name]

  create_bd_pin -dir I                  clk
  create_bd_pin -dir I -from 1023 -to 0 cfg
  create_bd_pin -dir O -from 15   -to 0 addr
  create_bd_pin -dir O -from 15   -to 0 addr_delayed
  create_bd_pin -dir O                  start

  # Add address counter
  cell xilinx.com:ip:c_counter_binary:12.0 base_counter \
    [list Output_Width [expr $bram_width+2] Increment_Value 4 SCLR true] \
    [list CLK clk Q addr]

  cell pavel-demin:user:edge_detector:1.0 reset_base_counter {} \
    [list clk clk dout base_counter/SCLR]

  cell xilinx.com:ip:xlslice:1.0 reset_base_counter_slice \
      [list DIN_WIDTH 1024 DIN_FROM $offset DIN_TO $offset] \
      [list Din cfg Dout reset_base_counter/din]

  cell xilinx.com:ip:xlslice:1.0 addr_delay_slice \
    [list DIN_WIDTH 1024 DIN_FROM [expr 3+2+$offset] DIN_TO [expr 2+$offset]] \
    [list Din cfg]

  set start_offset [expr $offset+1]
  cell xilinx.com:ip:xlslice:1.0 start_slice \
    [list DIN_WIDTH 1024 DIN_FROM [expr $start_offset] DIN_TO [expr $start_offset]] \
    [list Din cfg]

  cell pavel-demin:user:edge_detector:1.0 edge_detector {} [list din start_slice/Dout clk clk dout start]

  cell xilinx.com:ip:c_shift_ram:12.0 delay_addr \
    [list ShiftRegType Variable_Length_Lossless Width [expr $bram_width+2]] \
    [list D base_counter/Q CLK clk A addr_delay_slice/Dout Q addr_delayed]

  current_bd_instance $bd

}