/**
 * @author luis.rodriguez.soutullo
 */

package servicio;

import java.io.File;
import http.ServidorHTTP;

public class ServidorWeb {
    
    public static void main (String[] args) {
        ServidorHTTP servicio;
        
        if (args.length > 0)
            if (args.length == 1 && new File(args[0]).exists())
                servicio = new ServidorHTTP(args[0]);
            else{
                //throw new IllegalArgumentException();
                System.err.println("FORMATO: ServidorWeb [CONFIG_PATH]");
                
                return;
            }
        else
            servicio = new ServidorHTTP();
        
        servicio.escuchar();
    }
}
