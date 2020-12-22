package Main;

import java.util.ArrayList;
import java.util.List;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Collections;
import java.util.Comparator;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;
import java.awt.*;
import java.awt.event.*;

public class Main {
    public static void main(String[] args) {
        try{
            Main m = new Main();
            m.run(args);
        }catch(Exception e){
            System.out.println("Algo haido mal durante la ejecucion del programa.");
        }        
    }
    public void run(String[] args){
        Procesador p = Procesador.getProcesador();
        /*
        El programa recive como parámetro: -nogui, que inhabilita la interfaz grafica
        */
        try{
            switch (args.length) {
                case 0:
                case 1:
                    // ./gestormemoria -gui
                    if(args[1].compareTo("-gui")!=0) throw new WrongArgumentsException();
                    p.GUI_boot();
                    break;
                case 2:
                    // ./gestormemoria -nogui
                    if(args[1].compareTo("-nogui")!=0) throw new WrongArgumentsException();
                    p.CLI_boot();
                    break;
                case 5:    
                    // ./gestormemoria -nogui -f [archivo.txt]
                    if(args[1].compareTo("-nogui")!=0) throw new WrongArgumentsException();
                    if(args[2].compareTo("-f")!=0) throw new WrongArgumentsException();
                    if(args[4].compareTo("-m")!=0) if(args[4].compareTo("-p")!=0) throw new WrongArgumentsException();
                    p.CLI_boot(args);
                    break;
                default:
                    throw new WrongArgumentsException();
            }
        }catch(WrongArgumentsException e){        
            System.out.println("La sintaxis correcta para la ejecucion del programa es:\n"
                    + "Interfaz grafica(opcion por defecto) -->  ./gestormemoria -gui\n"
                    + "Terminal de comandos guiada -->  ./gestormemoria -nogui\n"
                    + "Terminal de salida autonoma --> ./gestormemoria -nogui -f [archivo.txt] [-m/-p]\n"
                    + "**m: mejor hueco | p: peor hueco  " + args.length);
        }
    }
    public class WrongArgumentsException extends Exception{

        /**
         * Hola, esto es una excepcion
         */
        public WrongArgumentsException(){
            super();
        }
    }
    public static class Procesador{
        public static final int MAX_MEM = 2000;
        public static final int MIN_ASS_MEM = 100;
        private static final String OUTPUT = "particiones.txt";
        private List<Event> events;
        private static Procesador p = null;
        private Procesador(){
            this.events = new ArrayList<>();
        }
        public static Procesador getProcesador(){
            return ( Procesador.p==null ? p=new Procesador() : p );
        }
        public void GUI_boot(){
            Ventana v = new Ventana(Procesador.p);
            v.showFieldDialog();
            v.askAlgor();
            v.animateAlgor();
        };
        public void CLI_boot(){
            Scanner sc = new Scanner(System.in);
            do {                
                System.out.println("Introduce la direccion del archivo que contiene los procesos a simular: ");
                String file = sc.nextLine();
                System.out.println("Que algoritmo deseas aplicar? Mejor Hueco: M | Peor Hueco: P ");
                String opts = sc.nextLine().toLowerCase();
                int opt = ( opts.compareTo("m")==0 ? 0 : ( opts.compareTo("p")==0 ? 1 : -1 ) );
                this.executeAlgorithms(file, opt);
                System.out.println("Los resultados los puedes consultar en el archivo: " + Procesador.OUTPUT);
                System.out.println("Quieres visualizarlos ahora? [S/N]");
                if(sc.nextLine().toLowerCase().compareTo("s")==0){
                    for(int i=0;i<events.size();i++){
                        System.out.println(events.get(i).getOrder()+" ");
                        List<Proceso> list = events.get(i).getList();
                        for(int j=0;j<list.size();j++){
                            System.out.println(list.get(j).toString() + " ");
                        }
                    }
                }
                System.out.println("Quieres introducir nuevos procesos? Se borrara el contenido de "+Procesador.OUTPUT + " [S/N]");
                if(sc.nextLine().toLowerCase().compareTo("n")==0) break;
            } while (true);
        };
        public void CLI_boot(String[] args){
            String file = args[3];
            int opt = ( args[4].compareTo("-m")==0 ? 0 : ( args[4].compareTo("-p")==0 ? 1 : -1 ) );
            this.executeAlgorithms(file, opt);
        };
        private void executeAlgorithms(String file, int opt){
            try{
                this.readFile(file);
                switch(opt){
                    case 0:
                        this.mejorHueco();
                        break;
                    case 1:
                        this.peroHueco();
                        break;
                    default:
                        throw new RuntimeException();
                }
                this.saveResults();
            }catch(IOException e){
                System.out.println("Ha habido un problema con el archivo de entrada o con el archivo de salida.");
            }
        }
        private void saveResults() throws IOException{
            File f = new File(OUTPUT);
            if(f.createNewFile()){
                try (FileWriter fw = new FileWriter(f)) {
                    for(int i=0;i<events.size();i++){
                        fw.write(events.get(i).getOrder()+" ");
                        List<Proceso> list = events.get(i).getList();
                        for(int j=0;j<list.size();j++){
                            fw.write(list.get(j).toString() + " ");
                        }
                        fw.write("\n");
                    }
                }
            }
        }
        private void readFile(String filename) throws IOException{
            File f = new File(filename);
            try (Scanner sc = new Scanner(f)) {
                Event e = new Event(0);
                while(sc.hasNextLine()){
                    Proceso pp = new Proceso();
                    pp.setName(sc.next());
                    pp.setArrival(sc.nextInt());
                    pp.setRequiredMem(sc.nextInt());
                    pp.setExecTime(sc.nextInt());
                    e.addElement(pp);
                }
                events.add(e);
            }
        }
        private void peroHueco(){
            this.peorHueco(0);
        }
        private void mejorHueco(){
            this.mejorHueco(0);
        }
        @SuppressWarnings("Convert2Lambda")
        private void mejorHueco(int waiting_time){
            try{
                List<Proceso> runnign_proces = new ArrayList<>();
                List<Hueco> huecos = new ArrayList<>();
                int i = 0, j = 0;
                List<Proceso> list_p = events.get(0).getList();
                Collections.sort(list_p, new Comparator<Proceso>(){
                    @Override
                    @SuppressWarnings("UnnecessaryBoxing")
                    public int compare(Proceso p1, Proceso p2){
                        return new Integer(p1.get_at()).compareTo(new Integer(p2.get_at()));
                    }
                });
                huecos.add(new Hueco(0, Procesador.MAX_MEM));
                Proceso pp = list_p.get(j++);
                while(true){
                    for(Proceso pa: runnign_proces){
                        if( i + pa.get_at() >= pa.exec_time ){
                            Hueco h1 = new Hueco(pa.mounting_point, pa.required_mem);
                            huecos.add(h1);
                            runnign_proces.remove(pa);
                        }
                    }
                    Collections.sort(huecos, new Comparator<Hueco>(){
                        @Override
                        @SuppressWarnings("UnnecessaryBoxing")
                        public int compare(Hueco p1, Hueco p2){
                            return new Integer(p1.get_mp()).compareTo(new Integer(p2.get_mp()));
                        }
                    });
                    for(Hueco h1: huecos){
                        huecos.stream().filter(h2 -> !(h1.get_mp()==h2.get_mp())).filter(h2 -> ((h1.get_mp()+h1.get_rm())==h2.get_mp()-1)).map((var h2) -> {
                            h1.setRequiredMem(h1.get_rm() + h2.get_rm());
                            return h2;
                        }).forEachOrdered(h2 -> {
                            huecos.remove(h2);
                        });
                    }
                    if(pp.get_at() == i){
                        Hueco h = huecos.get(0);
                        for(Hueco h1: huecos){
                            if(h1.get_mp() == h.get_mp()) continue;
                            if(h1.get_rm() < h.get_rm())
                                if(pp.get_rm() <= h1.get_rm())
                                    h = h1;
                        }
                        pp.setMountingPoint(h.get_mp());
                        if(pp.get_rm() == h.get_rm()) huecos.remove(h);
                        else{ h.setMountingPoint(pp.get_rm()); h.setRequiredMem(h.get_rm() - pp.get_rm());}
                        runnign_proces.add(pp);
                        if(j < list_p.size()) 
                                pp = list_p.get(j++);
                    }
                    Event e = new Event(i);
                    runnign_proces.forEach(pa -> {
                        e.addElement(pa);
                    });
                    events.add(e);
                    i++;
                    TimeUnit.MILLISECONDS.sleep(waiting_time);
                }
            }catch(InterruptedException e){
                System.err.println("LA EJECUCION SE HA CANCELADO DEBIDO A UN TIEMPO DE ESPERA QUE EXCEDE LOS LIMITES DE LA MAQUINA");
            }
        }
        @SuppressWarnings({"UnnecessaryBoxing", "Convert2Lambda"})
        private void peorHueco(int waiting_time){
            try{
                List<Proceso> runnign_proces = new ArrayList<>();
                List<Hueco> huecos = new ArrayList<>();
                int i = 0, j = 0;
                List<Proceso> list_p = events.get(0).getList();
                Collections.sort(list_p, (Proceso p1, Proceso p2) -> new Integer(p1.get_at()).compareTo(new Integer(p2.get_at())));
                huecos.add(new Hueco(0, Procesador.MAX_MEM));
                Proceso pp = list_p.get(j++);
                while(true){
                    for(Proceso pa: runnign_proces){
                        if( i + pa.get_at() >= pa.exec_time ){
                            Hueco h1 = new Hueco(pa.mounting_point, pa.required_mem);
                            huecos.add(h1);
                            runnign_proces.remove(pa);
                        }
                    }
                    Collections.sort(huecos, new Comparator<Hueco>(){
                        @Override
                        public int compare(Hueco p1, Hueco p2){
                            return new Integer(p1.get_mp()).compareTo(new Integer(p2.get_mp()));
                        }
                    });
                    for(Hueco h1: huecos)
                        huecos.stream().filter(h2 -> !(h1.get_mp()==h2.get_mp())).filter(h2 -> ((h1.get_mp()+h1.get_rm())==h2.get_mp()-1)).map(h2 -> {
                            h1.setRequiredMem(h1.get_rm() + h2.get_rm());
                            return h2;
                        }).forEachOrdered(h2 -> {
                            huecos.remove(h2);
                        });
                    if(pp.get_at()==i){
                        int max = huecos.get(0).get_rm();
                        int index = 0;
                        for(Hueco h1: huecos){
                            if(h1.get_rm()>max){
                                max = h1.get_rm();
                                index = huecos.indexOf(h1);
                            }
                        }
                        Hueco h = huecos.get(index);
                        if(h.get_rm()>pp.get_rm()){
                            pp.setMountingPoint(Procesador.MAX_MEM - h.get_rm());
                            runnign_proces.add(pp);
                            h.setRequiredMem(h.get_rm() - pp.get_rm());
                            h.setMountingPoint(pp.get_rm());
                            if(j < list_p.size()) 
                                pp = list_p.get(j++);
                        }
                    }
                    Event e = new Event(i);
                    runnign_proces.forEach(pa -> {
                        e.addElement(pa);
                    });
                    events.add(e);
                    i++;
                    TimeUnit.MILLISECONDS.sleep(waiting_time);
                }
            }catch(InterruptedException e){
                System.err.println("LA EJECUCION SE HA CANCELADO DEBIDO A UN TIEMPO DE ESPERA QUE EXCEDE LOS LIMITES DE LA MAQUINA");
            }
        }
    }
    public static class Event{
        private int num=-1; // numero del evento, orden cronologico
        private List<Proceso> list;
        
        public Event(int n){
            this.num = n;
            this.list = new ArrayList<>();
        }
        public void addElement(Proceso p){
            this.list.add(p);
        }
        public List<Proceso> getList(){
            return list;
        }
        public int getOrder(){
            return num;
        }
        public void setOrder(int n){
            this.num = n;
        }
    }
    public static class Proceso{
        private String name = null;
        private int arrival = 0;
        private int required_mem = 0;
        private int exec_time = 0;
        private int mounting_point=-1;
        
        public Proceso(){
            this.name = new String();
        }
        
        public int get_mp(){
            return this.mounting_point;
        }
        public int get_rm(){
            return this.required_mem;
        }
        public int get_at(){
            return this.arrival;
        }
        
        public void setMountingPoint(int mp){
            if(mp>=0) this.mounting_point = mp;
        }
        public void setName(String s){
            this.name = s;
        }
        public void setArrival(int a){
            this.arrival = a;
        }
        public void setRequiredMem(int m){
            this.required_mem = m;
        }
        public void setExecTime(int e){
            this.exec_time = e;
        }
        
        @Override
        public String toString(){
            return "[" + this.mounting_point + this.name + this.required_mem  + "]";
        }
    }
    public static class Hueco extends Proceso{
        public Hueco(){
            super();
        }
        @SuppressWarnings("OverridableMethodCallInConstructor")
        public Hueco(int start, int space){
            super();
            this.setMountingPoint(start);
            this.setRequiredMem(space);
        }
        @Override
        public String toString(){
            return "[" + this.get_mp() + "hueco" + this.get_rm()  + "]";
        }
    }
    @SuppressWarnings("FieldMayBeFinal")
    public static class Ventana {
        private Frame frame = new Frame("gestormemoria - GUI");
        private Panel panel;
        private Label headerL, statusL;
        private String filename;
        private Procesador p;
        public Ventana( Procesador p ){
            this.p = p;
            frame.setSize(400,400);
            frame.setLayout(new GridLayout(3, 1));
            frame.setLocationRelativeTo(null);
            frame.addWindowListener( new WindowAdapter() {
                @Override
                public void windowClosing( WindowEvent we ){
                    System.exit(0);
                }
            });
            headerL = new Label();
            headerL.setAlignment(Label.CENTER);
            statusL = new Label();
            statusL.setAlignment(Label.CENTER);
            panel = new Panel(new FlowLayout());
            frame.add(panel);
            frame.add(headerL);
            frame.add(statusL);
        }
        private void showFieldDialog(){
            headerL.setText("Escoge el archivo desde el qual cargar los procesos a simular:");
            final FileDialog fd = new FileDialog(frame,"Seleccionar");
            Button showFD = new Button("Seleccionar archivo");
            showFD.addActionListener((ActionEvent e) -> {
                fd.setVisible(true);
                filename = fd.getDirectory()+ fd.getFile();
                statusL.setText("Archivo seleccionado: " + filename );
            });
            panel.add(showFD);
            frame.setVisible(true);
        }
        private void askAlgor(){
            frame.removeAll();
            frame.setVisible(false);
            frame.setLayout( new GridLayout(2, 2));
            Button mb = new Button("Mejor Hueco");
            mb.addActionListener((ActionEvent e) -> {
                p.executeAlgorithms(filename, 0);
            });
            Button pb = new Button("Mejor Hueco");
            pb.addActionListener((ActionEvent e) -> {
                p.executeAlgorithms(filename, 1);
            });
            
            Label lable = new Label("Selecciona el algoritmo a aplicar", Label.CENTER);
            Label mt = new Label();
            frame.add(lable);
            frame.add(mt);
            frame.add(mb);
            frame.add(pb);
            frame.setVisible(true);
        }
        private void animateAlgor(){
            frame.removeAll();
            frame.setVisible(false);
            
            //TODOa
            
            frame.setVisible(true);
        }
    }
}
