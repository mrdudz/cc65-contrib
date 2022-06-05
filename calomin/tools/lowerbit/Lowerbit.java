import java.io.*;

public class Lowerbit {
        
    public static void main(String[] args) throws Exception{
        
        if (args.length!=2) {
            System.out.println("This program must be run with 2 arguments");
            return;
        }
        
        RandomAccessFile inf = new RandomAccessFile(args[0],"r");
        RandomAccessFile outf = new RandomAccessFile(args[1],"rw");
        
        long l =inf.length();
        /*Open the input file and read all data*/
        long ofs=0;
        int p1,p2;
        while (ofs<l) {
            p1 = inf.readUnsignedByte();
            ofs++;
            if (!(ofs<l)) p2=0; else p2 = inf.readUnsignedByte();
            ofs++;
            /*Now we have two bytes and we will merge them into one*/
            int b=0;
            b+= (p1 & 48) << 2;
            b+= (p1 &  3) << 4;
            b+= (p2 & 48) >> 2;
            b+= (p2 &  3);
            outf.writeByte(b);
            //System.out.println(Integer.toString(b)+","+Integer.toString(p1)+","+Integer.toString(p2));
            
        
        }
        inf.close();
        outf.close();
        
        
    }
    
}
