
void kmain() {

    // bardzo prymitywne wypisanie danych do terminala, później bedzie obsługiwane przez driver VGA
    char * to_print = "hello from c";
    int to_print_length = 13;
    char color = 0x0f;
    char* addr = (char*)0xb8000;
    for (int i = 0; i < to_print_length; i++)
    {
        *addr = to_print[i];
        addr++;
        *addr = color;
        addr++;
    }


    asm("hlt");
}