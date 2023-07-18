package pt.ulisboa.tecnico.classes.student;

import java.util.Scanner;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import io.grpc.InternalChannelz.ServerList;
import pt.ulisboa.tecnico.classes.Stringify;
import pt.ulisboa.tecnico.classes.contract.student.StudentClassServer.*;
import pt.ulisboa.tecnico.classes.contract.student.StudentServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;

import pt.ulisboa.tecnico.classes.contract.naming.ClassServerNamingServer;
import static pt.ulisboa.tecnico.classes.contract.student.StudentServiceGrpc.newBlockingStub;
// added
import pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.naming.ClassServerNamingServer.LookupRequest;

import static pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc.newBlockingStub;

import java.util.*;

public class StudentFrontend {

    private static final String ENROLL_CMD = "enroll";
	private static final String LIST_CMD = "list";
    private static final String EXIT_CMD = "exit";
    private static final String serviceName = "turmas";

    List<String> primaryServer = Collections.singletonList("P");
    List<String> secondaryServer = Collections.singletonList("S");

    private StudentServiceGrpc.StudentServiceBlockingStub stub;
    static NamingServerServiceGrpc.NamingServerServiceBlockingStub namingServerStub;	
    pt.ulisboa.tecnico.classes.student.Student student;
    pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.Student studentGrpc;
    //pt.ulisboa.tecnico.classes.contract.naming.ClassServer_NamingServer.Server server;

    // added
    pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.ServerE namingServerServiceGrpc;

    private ManagedChannel channel = null;

    public StudentFrontend(ManagedChannel namingServerChannel, String name, String id){
        
        student = new Student(name, id);
        // Create a blocking stub.

        studentGrpc = studentGrpc.newBuilder().setStudentId(id).setStudentName(name).build();
        // isto nao é suposto pq o servidor de nomes já está criado ?
        // namingServerServiceGrpc = namingServerServiceGrpc.newBuilder().setHost("namingServer").setPort(5000).build();
        // added
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

            if (ENROLL_CMD.equals(line)) {

                chooseRandomServer(primaryServer);
                
                if ( channel != null )
                    stub = StudentServiceGrpc.newBlockingStub(channel);
                else 
                    System.out.println("NamingServer unavailable");
                
                if (stub != null){
                    ResponseCode code = stub.enroll(EnrollRequest.newBuilder().setStudent(studentGrpc).build()).getCode();
                    
                    System.out.println(Stringify.format(code));
                    System.out.printf("\n");
                }
            }

            else if (LIST_CMD.equals(line)){

                chooseRandomServer(secondaryServer);

                if (channel != null)
                    stub = StudentServiceGrpc.newBlockingStub(channel);
                else 
                    System.out.println("NamingServer unavailable");

                if ( stub != null ){
                    ListClassResponse response = stub.listClass(ListClassRequest.getDefaultInstance());
                    
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
