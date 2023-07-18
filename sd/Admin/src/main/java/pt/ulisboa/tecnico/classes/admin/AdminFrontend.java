package pt.ulisboa.tecnico.classes.admin;

import java.util.Collections;
import java.util.List;
import java.util.Random;
import java.util.Scanner;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import pt.ulisboa.tecnico.classes.Stringify;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.admin.AdminClassServer.*;
import pt.ulisboa.tecnico.classes.contract.admin.AdminServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.professor.ProfessorClassServer;
import pt.ulisboa.tecnico.classes.contract.student.StudentClassServer;

import pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.naming.ClassServerNamingServer.LookupRequest;

import static pt.ulisboa.tecnico.classes.contract.admin.AdminServiceGrpc.newBlockingStub;
import static pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc.newBlockingStub;

public class AdminFrontend {

    private static final String ACTIVATE_CMD = "activate";
	private static final String DEACTIVATE_CMD = "deactivate";
	private static final String DUMP_CMD = "dump";
    private static final String EXIT_CMD = "exit";
    private static final String DEACTIVATE_GOSSIP = "deactivateGossip";
    private static final String ACTIVATE_GOSSIP = "activateGossip";
    private static final String GOSSIP = "gossip";
    private static final String serviceName = "turmas";

    List<String> primaryServer = Collections.singletonList("P");
    List<String> secondaryServer = Collections.singletonList("S");

    private AdminServiceGrpc.AdminServiceBlockingStub stub;
    static NamingServerServiceGrpc.NamingServerServiceBlockingStub namingServerStub;	

    private ManagedChannel channel = null;
    
    public AdminFrontend(ManagedChannel namingServerChannel) {
        // Create a blocking stub.
        namingServerStub = NamingServerServiceGrpc.newBlockingStub(namingServerChannel);
    }

    public void chooseRandomServer(List<String> qualifiersList) throws StatusRuntimeException{
        
        try {
            if ( namingServerStub != null ){
                List<ServerE> serverList = namingServerStub.lookup(LookupRequest.newBuilder().setName(serviceName).addAllQualifiersList(qualifiersList).build()).getServerPortsList();
                Random r = new Random();
                int low = 0;
                int high = serverList.size();
                int result = r.nextInt(high-low) + low;
                ServerE server = serverList.get(result);
                channel = ManagedChannelBuilder.forAddress(server.getHost(),server.getPort()).usePlaintext().build();
            }
           
        } catch (StatusRuntimeException e){
			System.out.println(e.getMessage());
		}
    }

    public void ScanInput() {
            Scanner scanner = new Scanner(System.in);

            while (true) {
                System.out.printf("> ");

                String line = scanner.nextLine();
                String[] arg = line.split(" ");
                int nArg = arg.length;

                if (ACTIVATE_CMD.equals(arg[0])) {
                    if (nArg != 2) {
                        System.out.println("Please enter appropriate arguments");
                        continue;
                    }
                    if ( arg[1].equals("P")){
                        chooseRandomServer(primaryServer);
                        if ( channel != null )
                            stub = AdminServiceGrpc.newBlockingStub(channel);
                        else 
                            System.out.println("NamingServer unavailable");
                    }
                    else if (arg[1].equals("S")){
                        chooseRandomServer(secondaryServer);
                        if ( channel != null )
                            stub = AdminServiceGrpc.newBlockingStub(channel);
                        else 
                            System.out.println("NamingServer unavailable");
                    }
                    else{
                        System.out.println("Invalid Arguments");
                        continue;
                    }

                    if ( stub != null ){
                        System.out.println(stub.activate(ActivateRequest.getDefaultInstance()).getCode());
                        System.out.printf("\n");
                    }
                }

                else if (DEACTIVATE_CMD.equals(arg[0])) {
                    if (nArg != 2) {
                        System.out.println("Please enter appropriate arguments");
                        continue;
                    }
                    if ( arg[1].equals("P")){
                        chooseRandomServer(primaryServer);
                        if ( channel != null )
                            stub = AdminServiceGrpc.newBlockingStub(channel);
                        else 
                            System.out.println("NamingServer unavailable");
                    }
                    else if (arg[1].equals("S")){
                        chooseRandomServer(secondaryServer);
                    if ( channel != null )
                        stub = AdminServiceGrpc.newBlockingStub(channel);
                    else 
                        System.out.println("NamingServer unavailable");
                    }

                    System.out.println(stub.deactivate(DeactivateRequest.getDefaultInstance()).getCode());
                    System.out.printf("\n");
                }

                else if (DUMP_CMD.equals(line)){
                    chooseRandomServer(primaryServer);
                    if ( channel != null )
                        stub = AdminServiceGrpc.newBlockingStub(channel);
                    else 
                        System.out.println("NamingServer unavailable");
                    
                    if ( stub != null ){
                        DumpResponse response = stub.dump(DumpRequest.getDefaultInstance());

                        ClassState classState = response.getClassState();
                        ResponseCode code = response.getCode();
                        if ( code != ResponseCode.OK ){
                            System.out.println(Stringify.format(code));
                            continue;
                        }
                        System.out.println(Stringify.format(classState));
                        System.out.printf("\n");
                    }
                }

                else if (DEACTIVATE_GOSSIP.equals(arg[0])) {
                    if (nArg == 2) {
                        if ( arg[1].equals("P")){
                            chooseRandomServer(primaryServer);
                            if ( channel != null )
                                stub = AdminServiceGrpc.newBlockingStub(channel);
                            else 
                                System.out.println("NamingServer unavailable");
                        }
                        else if (arg[1].equals("S")){
                            chooseRandomServer(secondaryServer);
                            if ( channel != null )
                                stub = AdminServiceGrpc.newBlockingStub(channel);
                            else 
                                System.out.println("NamingServer unavailable");
                        }
                        else{
                            System.out.println("Invalid Arguments");
                            continue;
                        }
                    }

                    if ( stub != null ){
                        System.out.println(stub.deactivateGossip(DeactivateGossipRequest.getDefaultInstance()).getCode());
                        System.out.printf("\n");
                    }
                }

                else if (ACTIVATE_GOSSIP.equals(arg[0])) {
                    if (nArg == 2) {
                        if ( arg[1].equals("P")){
                            chooseRandomServer(primaryServer);
                            if ( channel != null )
                                stub = AdminServiceGrpc.newBlockingStub(channel);
                            else 
                                System.out.println("NamingServer unavailable");
                        }
                        else if (arg[1].equals("S")){
                            chooseRandomServer(secondaryServer);
                            if ( channel != null )
                                stub = AdminServiceGrpc.newBlockingStub(channel);
                            else 
                                System.out.println("NamingServer unavailable");
                        }
                        else{
                            System.out.println("Invalid Arguments");
                            continue;
                        }
                    }

                    if ( stub != null ){
                        System.out.println(stub.activateGossip(ActivateGossipRequest.getDefaultInstance()).getCode());
                        System.out.printf("\n");
                    }
                }

                else if (GOSSIP.equals(arg[0])) {
                    
                    if ( arg[1].equals("P")){
                        
                    }
                    else if (arg[1].equals("S")){

                    }
                }

                else if(EXIT_CMD.equals(line)){
                    System.exit(0);
                }

                else {
                    System.out.println("Invalid argument");
                    System.out.printf("\n");
                }

            }
    }
}
