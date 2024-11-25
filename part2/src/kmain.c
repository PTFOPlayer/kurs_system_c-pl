void kmain() {
    char *buff = (char *)0xb8000;
    char *hello = "hello from c";
    while (*hello != '\0') {
        *buff = *hello;
        buff++;
        *buff = 0x0f;
        buff++;
        hello++;
    }

    while (1)
    {}

    asm("hlt");
    
}