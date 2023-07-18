package pt.ulisboa.tecnico.classes.professor;

import java.util.Collections;
import java.util.List;
import java.util.Random;
import java.util.Scanner;
import java.util.StringTokenizer;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
import pt.ulisboa.tecnico.classes.Stringify;
import pt.ulisboa.tecnico.classes.contract.professor.ProfessorClassServer.*;
import pt.ulisboa.tecnico.classes.contract.professor.ProfessorServiceGrpc;
import  pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerClassServer.PropagateStateRequest;
import pt.ulisboa.tecnico.classes.contract.student.StudentClassServer;

import pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.naming.ClassServerNamingServer.*;

import static pt.ulisboa.tecnico.classes.contract.professor.ProfessorServiceGrpc.newBlockingStub;
import static pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc.newBlockingStub;

public class ProfessorFrontend {

    private static final String OPEN_CMD = "openEnrollments";
	private static final String CLOSE_CMD = "closeEnrollments";
	private static final String LIST_CMD = "list";
	private static final String CANCEL_CMD = "cancelEnrollment";
	private static final String EXIT_CMD = "exit";
    private static final String serviceName = "turmas";

    List<String> primaryServer = Collections.singletonList("P");
    List<String> secondaryServer = Collections.singletonList("S");

    private ProfessorServiceGrpc.ProfessorServiceBlockingStub stub;
    static NamingServerServiceGrpc.NamingServerServiceBlockingStub namingServerStub;

    private ManagedChannel channel = null;

    public ProfessorFrontend(ManagedChannel namingServerChannel){

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

    public void ScanInpup() {
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.printf("> ");

            String line = scanner.nextLine();
            String[] arg = line.split(" ");
            int nArg = arg.length;

            // exit
            if (OPEN_CMD.equals(arg[0])) {

                chooseRandomServer(primaryServer);

                if ( channel != null )
                    stub = ProfessorServiceGrpc.newBlockingStub(channel);
                else 
                    System.out.println("NamingServer unavailable");

                if (nArg != 2) {
                    System.out.println("Please enter appropriate arguments");
                    continue;
                }
                try{
                    int n = Integer.parseInt(arg[1]);
                }catch(Exception e ){
                    System.out.println("Please enter appropriate arguments");
                    continue;
                }

                String capacity = arg[1];

                if (stub != null){
                    ResponseCode code = stub.openEnrollments(OpenEnrollmentsRequest.newBuilder().setCapacity(Integer.parseInt(capacity)).build()).getCode();
                    System.out.println(Stringify.format(code));
                    System.out.printf("\n");
                }

            }

            else if (CLOSE_CMD.equals(line)) {

                chooseRandomServer(primaryServer);

                if (channel != null)
                    stub = ProfessorServiceGrpc.newBlockingStub(channel);
                else 
                    System.out.println("NamingServer unavailable");

                if ( stub != null ){
                    CloseEnrollmentsResponse response = stub.closeEnrollments(CloseEnrollmentsRequest.getDefaultInstance());
                    ResponseCode code = response.getCode();

                    System.out.println(Stringify.format(code));
                    System.out.printf("\n");
                }

            }

            else if (LIST_CMD.equals(line)){

                chooseRandomServer(secondaryServer);

                if ( channel != null )
                    stub = ProfessorServiceGrpc.newBlockingStub(channel);
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

            else if (CANCEL_CMD.equals(arg[0])){

                chooseRandomServer(primaryServer);

                if ( channel != null )
                    stub = ProfessorServiceGrpc.newBlockingStub(channel);
                else 
                    System.out.println("NamingServer unavailable");
                
                if ( nArg != 2) {
                    System.out.println("Please enter appropriate arguments");
                    continue;
                }
                
                String aluno = arg[1];

                if ( stub != null ){
                    CancelEnrollmentResponse response = stub.cancelEnrollment(CancelEnrollmentRequest.newBuilder().setStudentId(aluno).build());
                    ResponseCode code = response.getCode();

                    System.out.println(Stringify.format(code));
                    System.out.printf("\n");
                }

            }

            else if(EXIT_CMD.equals(arg[0])){
                System.exit(0);
            }
            else {
                System.out.println("Invalid argument");
                System.out.printf("\n");
            }
        }
    }
}

