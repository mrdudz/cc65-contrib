
import java.io.*;
import java.util.*;


public class Minilinker {
    
    
    
    
    public static void main(String[] args) throws Exception {
        
        int curArg;
        int curAdr;
        
        
        /*Argument number check*/
        if (args.length % 2==1 || args.length <4 ) {
            System.out.println("Error: invalid number of arguments");
            return;
        }
        
        RandomAccessFile of = new RandomAccessFile(args[1],"rw");
        of.setLength(0);
        of.write(255);
        of.write(255);
        
        curArg=2;
        
        while(curArg<args.length) {
            
            /*Get adress*/
            curAdr=Integer.parseInt(args[curArg]);
            curArg++;
            /*Get size*/
            File inf = new File(args[curArg]);
            /*Open the file and read data into b*/
            RandomAccessFile tf = new RandomAccessFile(inf,"r");
            byte[] b=new byte[(int)inf.length()];
            tf.readFully(b);
            if (curAdr!=0) {
                /*Write a section into output file*/
                of.write(curAdr % 256);
                of.write(curAdr / 256);
                of.write((byte)((curAdr+inf.length()-1)%256));
                of.write((byte)((curAdr+inf.length()-1)/256));
            }
            of.write(b);
            tf.close();
            curArg++;
        }
        
        /*Now append the main file*/
        RandomAccessFile mf = new RandomAccessFile(args[0],"r");
        byte[] b2 = new byte[(int)mf.length()];
        mf.readFully(b2);
        mf.close();
        /*Write*/
        of.write(b2);
        of.close();
        
    }
    
}
