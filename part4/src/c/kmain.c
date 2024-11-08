
void kmain() {

    // bardzo prymitywne wypisanie danych do terminala, później bedzie obsługiwane przez driver VGA
    char * to_print = "hello from c";
    char color = 0x0f; // czarne tło biały tekst
    char* addr = (char*)0xb8000;
    int i = 0;
    while (to_print[i] != '\0')
    {
        *addr = to_print[i];
        addr++;
        *addr = color;
        addr++;

        i++;
    }


    asm("hlt");
}