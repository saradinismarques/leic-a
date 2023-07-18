package pt.ulisboa.tecnico.classes.namingserver;

import java.time.LocalTime;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;

public class NamingServices {

    public int server_id = 0;

    public NamingServices(){
    }

    private ConcurrentHashMap<String, ServiceEntry> namingServicesHashMap = new ConcurrentHashMap<>();

    public ConcurrentHashMap<String, ServiceEntry> getNamingServicesHashMap(){
        return this.namingServicesHashMap;
    }

    public int getServerId(){
        server_id += 1;
        return server_id;
    }


	public synchronized void register(String name, String host, int port, List<String> qualifiersList){
        int id = getServerId();
        ServerEntry serverEntry = new ServerEntry(host,port,qualifiersList,id);
        List<ServerEntry> serverEntryList = new ArrayList<ServerEntry>();
        serverEntryList.add(serverEntry);
        
        if (  namingServicesHashMap.containsKey(name) ){
            namingServicesHashMap.get(name).getServerEntryList().add(serverEntry);

        }
        else{
            ServiceEntry serviceEntry = new ServiceEntry(name, serverEntryList);
            namingServicesHashMap.put(name,serviceEntry);

        }

    }

    public synchronized List<ServerE> lookup(String name, List<String> qualifierList){
        
        List<ServerE> serverEntries = new ArrayList<ServerE>();
        ServiceEntry serviceEntry;
        
        if ( !namingServicesHashMap.containsKey(name)){
            return serverEntries;
        }

        else if ( qualifierList == null){

            serviceEntry = namingServicesHashMap.get(name);
            List<ServerEntry> serverEntryList = serviceEntry.getServerEntryList();
            for ( ServerEntry serverEntry : serverEntryList){
                ServerE aux = ServerE.newBuilder().setHost(serverEntry.getHost()).setPort(serverEntry.getPort()).addAllQualifiersList(serverEntry.getQualifiersList()).build();
                serverEntries.add(aux);
            }
            return serverEntries;

        }
        else{
            serviceEntry = namingServicesHashMap.get(name);
            List<ServerEntry> serverEntryList = serviceEntry.getServerEntryList();
            for ( ServerEntry serverEntry : serverEntryList ){
                List<String> qualifiersList = serverEntry.getQualifiersList();
                for ( String qualifier : qualifierList){
                    if ( qualifiersList.contains(qualifier) ){
                        ServerE aux = ServerE.newBuilder().setHost(serverEntry.getHost()).setPort(serverEntry.getPort()).addAllQualifiersList(serverEntry.getQualifiersList()).build();
                        serverEntries.add(aux);
                    }
                }
            }

            return serverEntries;
           
        }
    }

    public synchronized void delete(String name, String host, int port){
        
        if ( namingServicesHashMap.containsKey(name) ){
            ServiceEntry serviceEntry = namingServicesHashMap.get(name);
            List<ServerEntry> serverEntryList = serviceEntry.getServerEntryList();
            for ( ServerEntry serverEntry : serverEntryList ){
                if ( serverEntry.getHost().equals(host) && serverEntry.getPort() == port){
                    serverEntryList.remove(serverEntry);
                    break;
                } 
            }
        }

    }

    
}
