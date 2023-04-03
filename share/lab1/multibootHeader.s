.globl start 
magic_ITEM_NAME = 0x1BADB002
flag_ITEM_NAME = 0
checksum_ITEM_NAME = 0xE4524FFE

/*此处开始，按协议标准来定义必须的multiboot header*/
.section ".multiboot_header"
    .align 4 
    .long magic_ITEM_NAME
    .long flag_ITEM_NAME
    .long checksum_ITEM_NAME

.text 
.code32 
start: 
    # vga
    movl $0x2f422f50, 0xB8000 # BP
    movl $0x2f302f32, 0xB8004 # 02
    movl $0x2f302f30, 0xB8008 # 00
    movl $0x2f332f30, 0xB800C # 30
    movl $0x2f382f32, 0xB8010 # 82
    movl $0x2f792f5F, 0xB8014 # y_
    movl $0x2f682f62, 0xB8018 # hb
    
    
    
    
    /*PB20000328*/

    # uart
    # P
    movb $0x50, %al 
    movw $0x3F8, %dx
    outb %al, %dx 

    # B
    movb $0x42, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # 2
    movb $0x32, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # 0
    movb $0x30, %al  
    movw $0x3F8, %dx
    outb %al, %dx  

    # 0
    movb $0x30, %al 
    movw $0x3F8, %dx
    outb %al, %dx  

    # 0
    movb $0x30, %al  
    movw $0x3F8, %dx
    outb %al, %dx  

    # 0
    movb $0x30, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # 3
    movb $0x33, %al  
    movw $0x3F8, %dx
    outb %al, %dx  

    # 2
    movb $0x32, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # 8
    movb $0x38, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # _
    movb $0x5F, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # y
    movb $0x79, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # b
    movb $0x62, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # h
    movb $0x68, %al  
    movw $0x3F8, %dx
    outb %al, %dx 

    # halt
    hlt
