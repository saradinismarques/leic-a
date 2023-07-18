package pt.ulisboa.tecnico.classes.namingserver;

import java.util.List;

public class ServiceEntry {
    
    private String name;
    private List<ServerEntry> serverEntryList;

    public ServiceEntry(String name, List<ServerEntry> serverEntryList){
        this.name = name;
        this.serverEntryList = serverEntryList;
    }

    public String getServerEntryName(){
        return name;
    }

    public List<ServerEntry> getServerEntryList(){
        return serverEntryList;
    }
}
