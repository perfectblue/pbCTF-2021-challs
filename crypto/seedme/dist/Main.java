import java.nio.file.Files;
import java.nio.file.Path;
import java.io.IOException;
import java.util.Random;
import java.util.Scanner;

class Main {

    private static void printFlag() {
        try {
            System.out.println(Files.readString(Path.of("flag.txt")));
        }
        catch(IOException e) {
            System.out.println("Flag file is missing, please contact admins");
        }
    }

    public static void main(String[] args) {
        int unlucky = 03777;
        int success = 0;
        int correct = 16;

        System.out.println("Welcome to the 'Lucky Crystal Game'!");
        System.out.println("Please provide a lucky seed:");
        Scanner scr = new Scanner(System.in);
        long seed = scr.nextLong();
        Random rng = new Random(seed);

        for(int i=0; i<correct; i++) {
            /* Throw away the unlucky numbers */
            for(int j=0; j<unlucky; j++) {
                rng.nextFloat();
            }

            /* Do you feel lucky? */
            if (rng.nextFloat() >= (7.331f*.1337f)) {
                success++;
            }
        }

        if (success == correct) {
            printFlag();
        }
        else {
            System.out.println("Unlucky!");
        }
    }
}
