/**
 * @author luis.rodriguez.soutullo
 */

package http;

import java.net.Socket;
import java.net.ServerSocket;
import java.net.SocketTimeoutException;
import java.net.SocketException;
import java.io.IOException;
import java.util.Properties;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class ServidorHTTP {
    
    enum config {
        DEFAULT_PATH("webconfig"),
        PORT("5000"), 
        DIRECTORY_INDEX("index.html"), 
        DIRECTORY("webroot"), 
        ALLOW("false");
        
        private final String value;
        
        config(String value) {
            this.value = value;
        }
        
        public String getValue() {
            return this.value;
        }
    }
    
    private final ServerSocket servidor;
    private Socket cliente;
    private SolicitudHTTP peticion;
    
    private final int espera = 30000;
    
    public static int puerto;
    public static String indice;
    public static String raiz;
    public static boolean permitir;
    public static String versionHTTP;
    public static String nombre;
    public static String html400;
    public static String html404;
    public static String html403;
    public static String logAccesos;
    public static String logErrores;
    
    public ServidorHTTP() {
        try {
            configurar(config.DEFAULT_PATH.getValue());
            this.servidor = new ServerSocket(puerto);
            this.servidor.setSoTimeout(this.espera);
        
        }catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    
    public ServidorHTTP(String configPath) {
        try {
            configurar(configPath);
            this.servidor = new ServerSocket(puerto);
            this.servidor.setSoTimeout(this.espera);
        
        }catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    
    public void escuchar() {
        try {
            while (true) {
                // Aceptamos una nueva conexión
                this.cliente = this.servidor.accept();
                
                // Creamos un hilo de ejecución para la petición
                this.peticion = new SolicitudHTTP(this.cliente);
                
                // Atendemos la petición
                this.peticion.start();
            }
            
        } catch (SocketTimeoutException e) {
            System.err.println("Se ha superado el tiempo de espera.");

        } catch (SocketException e) {
            System.err.println(e.getMessage());
            
        } catch (IOException e) {
            throw new RuntimeException(e);

        } finally {
            try {
                this.servidor.close();
                
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }
    
    private void configurar(String configPath) {
        Properties prop = new Properties();
        File fConfig;
        FileInputStream fis = null;
        FileOutputStream fos = null;
        String tmp;
        
        try {
            if (!configPath.endsWith("/"))
                configPath += "/";
            
            fConfig = new File(configPath + "server.properties");
            fConfig.createNewFile();
            fis = new FileInputStream(fConfig);
            prop.load(fis);
            
            while ((tmp = prop.getProperty(config.PORT.name())) == null) {
                prop.setProperty(config.PORT.name(), config.PORT.getValue());
            }
            
            puerto = Integer.parseInt(tmp);
            
            while ((indice = prop.getProperty(config.DIRECTORY_INDEX.name())) == null)
                prop.setProperty(config.DIRECTORY_INDEX.name(), config.DIRECTORY_INDEX.getValue());
            
            if (indice.startsWith("/")) 
                indice = indice.substring(1);

            while ((raiz = prop.getProperty(config.DIRECTORY.name())) == null)
                prop.setProperty(config.DIRECTORY.name(), config.DIRECTORY.getValue());
            
            if (raiz.endsWith("/")) 
                raiz = raiz.substring(0, raiz.length() - 1);

            while ((tmp = prop.getProperty(config.ALLOW.name())) == null)
                prop.setProperty(config.ALLOW.name(), config.ALLOW.getValue());
            
            permitir = Boolean.parseBoolean(tmp);
            
            fos = new FileOutputStream(fConfig);
            prop.store(fos, "Propiedades del servidor");
            
            versionHTTP = "HTTP/1.0";
            nombre = "lrsWebServer/1.0";
            html400 = configPath + "error400.html";
            html404 = configPath + "error404.html";
            html403 = configPath + "error403.html";
            logAccesos = configPath + "accesos.log";
            logErrores = configPath + "errores.log";
            
        } catch (IOException e) {
            throw new RuntimeException(e);
        
        } finally {
            try {
                if (fis != null)
                    fis.close();
                if (fos != null)
                    fos.close();
            
            } catch (IOException e){
                throw new RuntimeException(e);
            }
        }
    }
    
}
