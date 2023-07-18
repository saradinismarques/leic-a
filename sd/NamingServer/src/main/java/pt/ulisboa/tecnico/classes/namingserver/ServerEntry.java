package pt.ulisboa.tecnico.classes.namingserver;

import java.util.List;

public class ServerEntry {

    private String host;
    private Integer port;
    private List<String> qualifiersList;
    private int id;
    // private List<LocalTime> vectorClock;

    public ServerEntry(String host, Integer port, List<String> qualifiersList, int id){
        this.host = host;
        this.port = port;
        this.qualifiersList = qualifiersList;
        this.id = id;
        // this.vectorClock = vectorClock;
    }

    public void setHost(String host){
        this.host = host;
    }

    public String getHost(){
        return host;
    }

    public void setPort(Integer port){
        this.port = port;
    }

    public Integer getPort(){
        return port;
    }

    public void setQualifiersList(List<String> qualifiersList){
        this.qualifiersList = qualifiersList;
    }

    public List<String> getQualifiersList(){
        return qualifiersList;
    }

    /*
    public void setVectorClock(List<LocalTime> vectorClock){
        this.vectorClock = vectorClock;
    }

    public List<LocalTime> getVectorClock(){
        return vectorClock;
    } */
    
}
