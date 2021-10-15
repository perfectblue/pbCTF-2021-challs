import com.seedfinding.latticg.reversal.DynamicProgram;
import com.seedfinding.latticg.util.LCG;
import com.seedfinding.latticg.reversal.calltype.java.JavaCalls;

class Solve {
    public static void main(String[] args) {
        int[] vals = new int[]{ 2047};
        float lim = 7.331f*.1337f;
        for(int i=0; i<vals.length; i++) {
            int val = vals[i];
            System.out.println(val);
            DynamicProgram device = DynamicProgram.create(LCG.JAVA);
            device.skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 1
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 2
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 3
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 4
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 5
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 6
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 7
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 8
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 9
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 10
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 11
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 12
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 13
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 14
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)) // 15
                .skip(val)
                .add(JavaCalls.nextFloat().greaterThan(lim)); // 16
            device.reverse().forEach(System.out::println); // Need to XOR with 0x5DEECE66D
        }
    }
}