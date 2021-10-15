STATIC_YOINK("__zip_start");

uLong flag[]= {  0x014400d3,
                0x042401aa,
                0x08bf028b,
                0x0efa034f,
                0x16a1040d,
                0x200004ea,
                0x2ae20597,
                0x3721065c,
                0x4507072b,
                0x542f07cd,
                0x651208a2,
                0x77860970,
                0x8b8f0a34,
                0xa0d50adf,
                0xb75c0b75,
                0xcfa40c5e,
                0xe9440d01,
                0x04520db2,
                0x20b10e6e
            };
    

void main(int argc, char** argv)
{
    if(argc!=2){ printf("Give flag\n"); }
    else{
        if(strlen(argv[1]) != 38){
            printf("No\n");
            exit(1);
        }
        uLong adl = adler32(0, Z_NULL, 0);
        for(int i=0; i<38; i=i+2)
        {
            adl = adler32(adl, &argv[1][i], 2);
            // printf("%#10lx\n", adl);
            if (adl != flag[i>>1])
            {
                printf("No\n");
                // printf("%#10lx\n", adl);
                // printf("%#10lx\n", flag[i>>2]);
                exit(1+(i>>1));
            }
            // printf("%#10lx\n", adl);
        }
        printf("Correct!\n");
        exit(0);
    }
}

// pbctf{acKshuaLLy_p0rtable_3x3cutAbLe?}