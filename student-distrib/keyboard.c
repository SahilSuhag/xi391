/*************************************************/
/* files.c - The terminal driver for the kernel. */
/*************************************************/
#include "lib.h"
#include "keyboard.h"
#include "i8259.h"



/* There are three char scan arrays, 0 = nothing, 1 = SHIFT, 2 = Caps, 3=Shift+Caps */
uint8_t kbd_chars[4][128] = {
	{
	    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 	'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',	
	 	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,'\\', 'z', 'x', 'c', 'v', 
	 	'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',	 0,
	    0,	/* 59 - F1 key ... > */
	    0,   0,   0,   0,   0,   0,   0,   0,
	    0,	/* < ... F10 */
	    0,	/* 69 - Num lock*/
	    0,	/* Scroll Lock */
	    0,	/* Home key */
	    0,	/* Up Arrow */
	    0,	/* Page Up */
	  '-',
	    0,	/* Left Arrow */
	    0,
	    0,	/* Right Arrow */
	  '+',
	    0,	/* 79 - End key*/
	    0,	/* Down Arrow */
	    0,	/* Page Down */
	    0,	/* Insert Key */
	    0,	/* Delete Key */
	    0,   0,   0,
	    0,	/* F11 Key */
	    0,	/* F12 Key */
	    0,	/* All other keys are undefined */
	},{
	    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 	'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,0, 'A', 'S',	
	 	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,'|', 'Z', 'X', 'C', 'V', 
	 	'B', 'N', 'M', '<', '>', '?', 0, '*', 0, 0,	 0,
	    0,	/* 59 - F1 key ... > */
	    0,   0,   0,   0,   0,   0,   0,   0,
	    0,	/* < ... F10 */
	    0,	/* 69 - Num lock*/
	    0,	/* Scroll Lock */
	    0,	/* Home key */
	    0,	/* Up Arrow */
	    0,	/* Page Up */
	  '-',
	    0,	/* Left Arrow */
	    0,
	    0,	/* Right Arrow */
	  '+',
	    0,	/* 79 - End key*/
	    0,	/* Down Arrow */
	    0,	/* Page Down */
	    0,	/* Insert Key */
	    0,	/* Delete Key */
	    0,   0,   0,
	    0,	/* F11 Key */
	    0,	/* F12 Key */
	    0,	/* All other keys are undefined */
	},{
		0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 	'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S',	
	 	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0,'\\', 'Z', 'X', 'C', 'V', 
	 	'B', 'N', 'M', ',', '.', '/', 0, '*', 0, 0,	 0,
	    0,	/* 59 - F1 key ... > */
	    0,   0,   0,   0,   0,   0,   0,   0,
	    0,	/* < ... F10 */
	    0,	/* 69 - Num lock*/
	    0,	/* Scroll Lock */
	    0,	/* Home key */
	    0,	/* Up Arrow */
	    0,	/* Page Up */
	  '-',
	    0,	/* Left Arrow */
	    0,
	    0,	/* Right Arrow */
	  '+',
	    0,	/* 79 - End key*/
	    0,	/* Down Arrow */
	    0,	/* Page Down */
	    0,	/* Insert Key */
	    0,	/* Delete Key */
	    0,   0,   0,
	    0,	/* F11 Key */
	    0,	/* F12 Key */
	    0,	/* All other keys are undefined */
	},{
	    
	    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 	'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '{', '}', 0,0, 'a', 's',	
	 	'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0,'|', 'z', 'x', 'c', 'v', 
	 	'b', 'n', 'm', '<', '>', '?', 0, '*', 0, 0,	 0,
	    0,	/* 59 - F1 key ... > */
	    0,   0,   0,   0,   0,   0,   0,   0,
	    0,	/* < ... F10 */
	    0,	/* 69 - Num lock*/
	    0,	/* Scroll Lock */
	    0,	/* Home key */
	    0,	/* Up Arrow */
	    0,	/* Page Up */
	  '-',
	    0,	/* Left Arrow */
	    0,
	    0,	/* Right Arrow */
	  '+',
	    0,	/* 79 - End key*/
	    0,	/* Down Arrow */
	    0,	/* Page Down */
	    0,	/* Insert Key */
	    0,	/* Delete Key */
	    0,   0,   0,
	    0,	/* F11 Key */
	    0,	/* F12 Key */
	    0,	/* All other keys are undefined */
	}	
};


uint32_t current_terminal;

/* The buffer of chars which make up the command. */
char command_buffer[3][TERMINAL_BUFFER_MAX_SIZE];

/* Length of the current command (buffer). */
uint32_t command_length[3];

/* The x position of the cursor. */
uint32_t cursor_x[3];

/* Flags associated with the key pressed.
 * bit 0: shift on or off
 * bit 1: caps on or off
 * bit 2: ctrl on or off 
 * bit 3: alt on or off */
unsigned char keyboardflag[3];

/* This lock on terminal reading allows us to send complete buffers for reading
 * by foribing terminal_read until ready (upon a MAKE_ENTER scancode). */
uint32_t allow_terminal_read[3];



/* 
 * terminal_read()
 *
 * Description:
 * Implements read syscall specific to the terminal. 
 *
 * Inputs:
 * buf: buf to read the command buffer into
 * nbytes: number of bytes to read
 *
 * Outputs:
 * countread: the number of bytes read
 */
int32_t terminal_read(unsigned char * buf, int32_t nbytes) {
	int i;
	int countread = 0;
	
	set_command_y(0);

	/* Spin until allow_terminal_read = 1 (we allow it to be read). */
	while(!allow_terminal_read[current_terminal]);

	new_line();

	/* Iterate through nbytes reading (putting) the command buffer into buf. */
	for (i = 0; i < nbytes; i++) {
		buf[i] = command_buffer[current_terminal][i];
		command_buffer[current_terminal][i] = NULL;
		countread++;
	}

	/* Reset command_length, cursor, and the allow_terminal_read lock. */
	command_length[current_terminal] = 0;
	cursor_x[current_terminal] = 0;
	allow_terminal_read[current_terminal] = 0;

	return countread;
}

/* 
 * terminal_write()
 *
 * Description:
 * Implements write syscall specific to the terminal. It prints a buffer with
 * "putc" and returns the number of bytes (or characters) printed.
 *
 * Inputs:
 * buf: input buffer
 * nbytes: number of bytes to print from buffer
 *
 * Outputs:
 * successputs: the number of bytes printed
 */
int32_t terminal_write(const unsigned char * buf, int32_t nbytes) {

	int i;

	/* Number of bytes printed begins at zero. */
	int successputs = 0;

	/* Iteratively print the buf to the screen. */
	for (i = 0; i < nbytes; i++) {

		/* Print a char from the buffer. */
		putit(buf[i], current_terminal, get_tty_num());

		/* Increment the number of bytes printed. */
		successputs++;

	}

	/* Return the number of bytes printed. */
	return successputs;
}

/* 
 * keyboard_open()
 *
 * Description:
 * Initializes the keyboard.
 *
 * Inputs: none
 *
 * Outputs: none
 */
void keyboard_open(void) {

	/* Initially zero the buffer. */
	int i,j; 

	for(i = 0; i < 3; i++) {
		for( j = 0; j < TERMINAL_BUFFER_MAX_SIZE; j++ ) {
			command_buffer[i][j] = 0;
		}
		
		/* Initially set the key input flags to 00 */
		keyboardflag[i] = 0x00;

		/* Forbit terminal read until a return */
		allow_terminal_read[i]  = 0;

		/* Initially command length is zero because nothing has been typed. */
		command_length[i] = 0;

		/* x position of the cursor is initially zero */
		cursor_x[i] = 0;
	}
	
	/* the beginning terminal is index 0 */
	current_terminal = 0;	

	set_command_y(0);
	update_cursor(7);

	/* Unmask IRQ1 */
	enable_irq(KEYBOARD_IRQ);
}



/*
 * get_active()
 */
int32_t get_active(void){
	return current_terminal;
}

/* 
 * place_character_at_index()
 *
 * Description:
 * Puts a character a certain x position on the current row. This may involve
 * shifting the rest of the line down to fit the character where the cursor
 * currently resides.
 *
 * Inputs:
 * scancode: the input byte from the keyboard
 * index: the x position
 *
 * Outputs: none
 */
void place_character_at_index(uint8_t scancode, int index) {
	
	/* The end of the line is set at first to be the same as command_length. */
    int end_of_line = command_length[current_terminal];
   
    int8_t datum;
	/* This character-to-be-added (datum) is retreieved from the 
	 * massive "kbd_chars" array. */
    datum = kbd_chars[keyboardflag[current_terminal] & 0x03][scancode];
   
    /* If the user is inserting the character before the last possible position,
	 * then the data on and in front of the cursor needs to move to the right to
	 * let the new character be placed at the new spot (where the cursor is). */
    while( index <= end_of_line){
            command_buffer[current_terminal][end_of_line+1] = command_buffer[current_terminal][end_of_line];
            end_of_line--;
    }
	
	/* Place character at the index of the command buffer. */
	command_buffer[current_terminal][index] = datum;

	/* Incrememnt command_length because it has just added a character. */
	command_length[current_terminal]++;

	/* Move the cursor by 1 x position because we have added a character. */
	cursor_x[current_terminal]++;
 }

/* 
 * printthebuffer()
 *
 * Description:
 * Iterates through the buffer and uses "putc" to print it.
 *
 * Inputs: none
 *
 * Outputs: none
 */
void printthebuffer(void) {
	int i;

	/* Return. */
	carriage_return();

	/* Iterate though command buffer and print each char. */
	for (i=0; i<=command_length[current_terminal]; i++) {
		putit(command_buffer[current_terminal][i], current_terminal, current_terminal);
	}

}

/* 
 * process_keyboard_input()
 *
 * Description:
 * This runs for every keypress. It handles the input based on its scancode.
 * Regular characters are placed into the command buffer. Other key inputs such
 * as backspace, delete, enter, ctrl, alt, and arrow keys perform their
 * respective tasks.
 *
 * Inputs:
 * scancode: byte of data retrieved from keyboard
 *
 * Outputs: none
 */
void process_keyboard_input(uint8_t scancode)
{
	uint8_t nextcode;
	uint32_t new_terminal;
	uint32_t curr_process;

	int32_t cursor_index = cursor_x[current_terminal];

	int i;

	/* Store the datum received from the keyboard port. */
	if ( (keyboardflag[current_terminal] & 0x04) == 0 &&
			((scancode >= MAKE_1 && scancode <= MAKE_EQUALS) ||
			 (scancode >= MAKE_Q && scancode <= MAKE_R_SQUARE_BRACKET) ||
			 (scancode >= MAKE_A && scancode <= MAKE_ACCENT) ||
			 (scancode >= MAKE_BACKSLASH && scancode <= MAKE_SLASH) ||
			  scancode == MAKE_SPACE)
			) {

		/* (Stop placing characters in the command_buffer if 
		 * command_length is big.) */
		if (command_length[current_terminal] < TERMINAL_BUFFER_MAX_SIZE) {
			/* Put the character into the buffer (shift data if necessary). */
			place_character_at_index(scancode, cursor_x[current_terminal]);
		}

	} else if (scancode == MAKE_ENTER) {
		
		/* Remove the lock on terminal reading. */
		allow_terminal_read[current_terminal] = 1;

	} else if (scancode == MAKE_BKSP) {

		/* Backspace, shifting data if necessary. */
		if (cursor_index > 0 ) {
			cursor_index--;
			while ( cursor_index < command_length[current_terminal]) {
				command_buffer[current_terminal][cursor_index] = command_buffer[current_terminal][cursor_index+1];
				cursor_index++;
			}
			command_length[current_terminal]--;
			cursor_x[current_terminal]--;
		}

	} else if (scancode == MAKE_DELETE) {

		/* Delete, shifting data if necessary. */
		if (cursor_index >= 0 && cursor_index < command_length[current_terminal]) {
			while (cursor_index < command_length[current_terminal]) {
				command_buffer[current_terminal][cursor_index] = command_buffer[current_terminal][cursor_index+1];
				cursor_index++;
			}
			command_length[current_terminal]--;
		}

	} else if (scancode == MAKE_CAPS) {

		/* Change the keyboard flags for +/- CAPSLOCK */
		keyboardflag[current_terminal] ^= 0x02;

	} else if (scancode == MAKE_L_SHFT || scancode == MAKE_R_SHFT) /* Shift down */ {

		/* Change the keyboard flags for + SHIFT */
		keyboardflag[current_terminal] |= 0x01;

	} else if (scancode == BREAK_L_SHFT || scancode == BREAK_R_SHFT) /* Shift up */	{

		/* Change the keyboard flags for - SHIFT */
		keyboardflag[current_terminal] &= ~0x01;

	} else if (scancode == MAKE_L_CTRL) /* Ctrl down */ {

		/* Change the keyboard flags for + CTRL */
		keyboardflag[current_terminal] |= 0x04;

	} else if (scancode == BREAK_L_CTRL) /* Ctrl up */ {

		/* Change the keyboard flags for - CTRL */
		keyboardflag[current_terminal] &= ~0x4;

	} else if (scancode == MAKE_L_ALT) /* Alt down */ {

		/* Change the keyboard flags for + ALT */
		keyboardflag[current_terminal] |= 0x08;

	} else if (scancode == BREAK_L_ALT) /* Alt up */ {

		/* Change the keyboard flags for - ALT */
		keyboardflag[current_terminal] &= ~0x8;

	} else if ( scancode >= MAKE_F1 && scancode <= MAKE_F3){

		if(keyboardflag[current_terminal] & 0x8){
			new_terminal = (scancode & 0x7) - 3;
			curr_process = get_tty_num();
			if( new_terminal != curr_process){
				current_terminal = new_terminal;
				load_video_memory(current_terminal);
			}
		}

	} else if (scancode == EXTRAS) /* Directional and RCTRL */ {

		/* Get next keyboard input */
		nextcode = inb(KEYBOARD_PORT);

		/* Move cursor based on arrow key input. */
		if (nextcode == MAKE_L_ARROW && cursor_x[current_terminal] > 0) {

			cursor_x[current_terminal]--;

		} else if (nextcode == MAKE_R_ARROW && cursor_x[current_terminal] < command_length[current_terminal] ) {

			cursor_x[current_terminal]++;

		} else if (nextcode == MAKE_L_CTRL) {

			/* Change the keyboard flags for + CTRL */
			keyboardflag[current_terminal] |=  0x04;

		} else if (nextcode == BREAK_L_CTRL) {

			/* Change the keyboard flags for - CTRL */
			keyboardflag[current_terminal] &= ~0x04;

		}

	} else if (keyboardflag[current_terminal] & 0x04) /* CTRL + L */ {

		/* Implement screen clear with CTRL + L input. */
		if (scancode == MAKE_L){
			for (i=0; i < command_length[current_terminal]; i++) {
				command_buffer[current_terminal][i] = NULL;
			}
			command_length[current_terminal] = 0;
			cursor_x[current_terminal] = 0;
			clear_the_screen();
			keyboardflag[current_terminal] &= ~0x4;
		}

	}

	update_cursor(cursor_x[current_terminal]);
}

/* 
 * keyboard_interruption()
 *
 * Description:
 * Triggered by a keyboard interrupt (routed from the idt).
 *
 * Inputs: none
 *
 * Outputs: none
 */
void keyboard_interruption() {
	
	/* Mask interrupts */
	cli();
	
	/* Scancode (byte) that we receive from the keyboard. */
	uint8_t keyboard_scancode;

	/* Status that we get from keyboard to see if the buffer is full. */
	uint8_t keyboard_status;

	do {
		/* Dequeue the typed character from the keyboard buffer. */
		keyboard_scancode = inb(KEYBOARD_PORT);
		
		/* Process the input from the keyboard. */
		process_keyboard_input(keyboard_scancode);

		/* Print the buffer */
		printthebuffer();
		
		/* Check to see if the keyboard buffer is full. */
		keyboard_status = inb(KEYBOARD_STATUS_PORT);

	/* If the buffer is still full, repeat the process until the buffer 
	 * is empty. */
	} while ((keyboard_status & 0x02) != 0x00);

	/* Send End-of-Interrupt */
	send_eoi(KEYBOARD_IRQ);

	/* Unmask interrupts */
	sti();

}
