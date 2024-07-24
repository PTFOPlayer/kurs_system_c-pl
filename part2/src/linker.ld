ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        /* 
            umieszczenie sekcji multiboot na początku programu
            z tego momentu zaczyna się działanie systemu
        */
        *(.multiboot_header)
    }

    .text :
    {
        /*
            główna sekcja programu
        */
        *(.text)
    }
}