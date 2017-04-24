/**
 * @author luis.rodriguez.soutullo
 */

package http;

import java.net.Socket;
import java.net.SocketException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Date;
import java.text.SimpleDateFormat;
import java.text.ParseException;
import java.util.Locale;
import java.util.ArrayList;
//import java.net.URLConnection;

public class SolicitudHTTP extends Thread {
    
    private class Cabeceras {
        private Date cDate;
        private String cHost;
        
        public Date getDate() {
            return this.cDate;
        }
        
        public void setDate(Date cDate) {
            this.cDate = cDate;
        }
        
        public String getHost() {
            return this.cHost;
        }
        
        public void setHost(String cHost) {
            this.cHost = cHost;
        }
    }
    
    enum tEstado {
        OK("200 OK"), 
        NOT_MODIFIED("304 Not Modified"),
        BAD_REQUEST("400 Bad Request"), 
        FORBIDDEN("403 Forbidden"),
        NOT_FOUND("404 Not Found");
        
        private final String value;
        
        tEstado(String value) {
            this.value = value;
        }
        
        public String getValue() {
            return this.value;
        }
    }
    
    private final Socket cliente;
    private final BufferedReader entrada;
    private final OutputStream salida;
    private final Cabeceras args;
    
    private String lnPeticion;
    
    public SolicitudHTTP(Socket cliente) {
        this.cliente = cliente;
        this.args = new Cabeceras();
        
        try {
            this.entrada = new BufferedReader(new InputStreamReader(cliente.getInputStream()));
            this.salida = cliente.getOutputStream();
        
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    
    @Override
    public void run() {
        String ln;
        ArrayList<String> lnsCabecera = new ArrayList<>();
        
        try {
            this.lnPeticion = this.entrada.readLine();
            
            if (this.lnPeticion != null) {
                System.out.println(this.lnPeticion);

                do {
                    ln = this.entrada.readLine();
                    lnsCabecera.add(ln);
                    System.out.println(ln);
                } while (!ln.isEmpty());

                interpretar(lnsCabecera);
                lnsCabecera.clear();
                
                switch (this.lnPeticion.split(" ")[0]) {
                    case "GET":
                        httpGET();
                        break;
                    case "HEAD":
                        httpHEAD();
                        break;
                    default:
                        respuesta(tEstado.BAD_REQUEST, null, true);
                }
            }
            
        } catch (SocketException e) {
            System.err.println(e.getMessage());
            
        } catch (IOException e) {
            throw new RuntimeException(e);
        
        } finally {
            try {
                this.entrada.close();
                this.salida.close();
                this.cliente.close();
            
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }
    
    private void interpretar(ArrayList<String> cabeceras) {
        SimpleDateFormat sdf;
        
        for (String cabecera : cabeceras) {
            switch (cabecera.split(": ")[0]) {
                case "If-Modified-Since":
                    try {
                        sdf = new SimpleDateFormat("EEE MMM dd HH:mm:ss z yyyy", new Locale("english"));
                        this.args.setDate(sdf.parse(cabecera.split(": ")[1]));

                    } catch (ParseException e) {
                        System.err.println(e.getMessage());
                    }

                break;
                case "Host":
                    this.args.setHost(cabecera.split(": ")[1]);
            }
        }
    }
    
    private void httpGET() {
        String ruta = this.lnPeticion.split(" ")[1];
        File recurso = new File(ServidorHTTP.raiz + ruta), recurso2;
        Date fModServidor = new Date(recurso.lastModified()), fModCliente;
        String URL;
        
        if (recurso.exists())
            if (recurso.isDirectory()) {
                if (!ruta.endsWith("/"))
                    ruta += "/";
                
                recurso2 = new File(ServidorHTTP.raiz + ruta + ServidorHTTP.indice);
                
                if (recurso2.exists())
                    respuesta(tEstado.OK, recurso2, true);
                else
                    if (ServidorHTTP.permitir){
                        URL = "http://" + this.args.getHost() + ruta;
                        enlazar(recurso.list(), URL);
                    }
                    else
                        respuesta(tEstado.FORBIDDEN, null, true);
            }
            else{    
                fModCliente = this.args.getDate();
                if (fModCliente == null || fModServidor.after(fModCliente))
                    respuesta(tEstado.OK, recurso, true);
                else
                    respuesta(tEstado.NOT_MODIFIED, null, false);
            }
        else
            respuesta(tEstado.NOT_FOUND, null, true);
    }
    
    private void httpHEAD() {
        String ruta = this.lnPeticion.split(" ")[1];
        File recurso = new File(ServidorHTTP.raiz + ruta);
        
        if (recurso.exists())
            respuesta(tEstado.OK, recurso, false);
        else
            respuesta(tEstado.NOT_FOUND, null, false);
    }
    
    private void respuesta(tEstado estado, File recurso, boolean cuerpo) {
        PrintWriter pr = new PrintWriter(salida, true);
        String lnEstado;
        
        // Escogemos el estado a enviar
        switch (estado) {
            case OK:
                lnEstado = ServidorHTTP.versionHTTP + " " 
                        + tEstado.OK.getValue();
                
                pr.println(lnEstado);
                
                // Enviamos las cabeceras
                pr.println("Server: " + ServidorHTTP.nombre);
                pr.println("Date: " + new Date());
                //pr.println("Content-Type: " + URLConnection.guessContentTypeFromName(fichero.getName()));
                if (recurso != null) {
                    pr.println("Content-Type: " + tipoContenido(recurso.getName()));
                    pr.println("Content-Length: " + recurso.length());
                    pr.println("Last-Modified: " + new Date(recurso.lastModified()));
                }
                
                break;
            case BAD_REQUEST:
                lnEstado = ServidorHTTP.versionHTTP + " " 
                        + tEstado.BAD_REQUEST.getValue();
                
                pr.println(lnEstado);
                recurso = new File(ServidorHTTP.html400);
                
                break;
            case NOT_FOUND:
                lnEstado = ServidorHTTP.versionHTTP + " " 
                        + tEstado.NOT_FOUND.getValue();
                
                pr.println(lnEstado);
                recurso = new File(ServidorHTTP.html404);
                
                break;
            case NOT_MODIFIED:
                lnEstado = ServidorHTTP.versionHTTP + " " 
                        + tEstado.NOT_MODIFIED.getValue();
                
                pr.println(lnEstado);
                pr.println("Date: " + new Date());
                pr.println("Server: " + ServidorHTTP.nombre);
                
                break;
            case FORBIDDEN:
                lnEstado = ServidorHTTP.versionHTTP + " " 
                        + tEstado.FORBIDDEN.getValue();
                
                pr.println(lnEstado);
                recurso = new File(ServidorHTTP.html403);
                
                break;
            default:
                throw new IllegalArgumentException();
        }
        
        // Enviamos la linea en blanco
        pr.println();
        
        // Transferimos el cuerpo de entidad (o no)
        if (cuerpo)
            transferir(recurso);
        
        escribirLog(lnEstado, recurso);
    }
    
    private void transferir(File recurso) {
        FileInputStream fis = null;
        byte[] kbyte = new byte[1024];
        int tam;
        
        try {
            fis = new FileInputStream(recurso);
            while ((tam = fis.read(kbyte)) != -1) {
                this.salida.write(kbyte, 0, tam);
            }
            
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
            
        } catch (IOException e) {
            throw new RuntimeException(e);
        
        } finally {
            if (fis != null)
                try {
                    fis.close();
                
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
        }
    }
    
    private String tipoContenido (String fNombre) {
        String fTipo, ext = fNombre.substring(fNombre.lastIndexOf(".") + 1);
        
        switch (ext) {
            case "html":
                fTipo = "text/html";
                break;
            case "txt":
                fTipo = "text/plain";
                break;
            case "gif":
                fTipo = "image/gif";
                break;
            case "jpg":
                fTipo = "image/jpeg";
                break;
            case "htm":
                fTipo = "text/html";
                break;
            case "jpeg":
                fTipo = "image/jpeg";
                break;
            default:
                fTipo = "application/octet-stream";
        }
        
        return fTipo;
    }
    
    private void enlazar (String[] ficheros, String URL) {
        String enlaces = "";
        
        enlaces += "<html>"
                + "<head></head>"
                + "<body>"
                + "<ul>";
        
        for (String fichero : ficheros) {
            enlaces += "<li>"
                    + "<a href=\"" + URL + fichero + "\">" + fichero + "</a>"
                    + "</li>";
        }
        
        enlaces += "</ul>"
                + "</body>"
                + "</html>";
        
        try {
            respuesta(tEstado.OK, null, false);
            this.salida.write(enlaces.getBytes());
        
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    
    private void escribirLog(String lnEstado, File recurso) {
        File log;
        PrintWriter pw;
        String codigo = lnEstado.split(" ")[1];
        
        try {
            if (codigo.startsWith("2") || codigo.startsWith("3")){
                log = new File(ServidorHTTP.logAccesos);
                pw = new PrintWriter(new FileOutputStream(log, true), true);
                
                pw.println("Peticion: " + this.lnPeticion);
                pw.println("IP: " + this.cliente.getInetAddress().getHostName());
                pw.println("Fecha/Hora: " + new Date());
                pw.println("Codigo de estado: " + codigo);
                if (recurso != null)
                    pw.println("Bytes enviados: " + recurso.length());
                pw.println();
            }
            else
                if (codigo.startsWith("4")) {
                    log = new File(ServidorHTTP.logErrores);
                    pw = new PrintWriter(new FileOutputStream(log, true), true);

                    pw.println("Peticion: " + this.lnPeticion);
                    pw.println("IP: " + this.cliente.getInetAddress().getHostName());
                    pw.println("Fecha/Hora: " + new Date());
                    pw.println("Mensaje: " + lnEstado);
                    pw.println();
                }
            
        } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
        }
    }
    
}
