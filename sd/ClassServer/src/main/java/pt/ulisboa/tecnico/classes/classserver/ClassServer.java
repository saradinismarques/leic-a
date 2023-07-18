package pt.ulisboa.tecnico.classes.classserver;

import java.io.IOException;
import java.time.LocalTime;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Timer;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.StatusRuntimeException;
import io.grpc.BindableService;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerClassServer.PropagateStateRequest;
import pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.naming.ClassServerNamingServer.*;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.admin.AdminServiceGrpc;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerClassServer.*;

import static pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc.newBlockingStub;
import static pt.ulisboa.tecnico.classes.contract.classserver.ClassServerServiceGrpc.newBlockingStub;

public class ClassServer {

	static NamingServerServiceGrpc.NamingServerServiceBlockingStub namingServerStub;
	static ClassServerServiceGrpc.ClassServerServiceBlockingStub gossipStub;	
	static ClassServerServiceGrpc.ClassServerServiceBlockingStub updateStub;

  	public static void main(String[] args) throws StatusRuntimeException {

		for (int i = 0; i < args.length; i++) {
			System.out.printf("args[%d] = %s%n", i, args[i]);
		}
	  
		// check arguments
		if (args.length < 2) {
			System.out.println("Argument(s) missing!");
			System.out.printf("Usage: java %s host port%n", ClassServer.class.getName());
			return;
		}


		
		List<String> secondaryServer = Collections.singletonList("S");
		List<String> primaryServer = Collections.singletonList("P");
		
		
		final String name = "turmas";
		final String host = args[0];
		final int port = Integer.parseInt(args[1]);
		final String qualifier = args[2];
		final List<String> qualifiersList = new ArrayList<String>();
		qualifiersList.add(qualifier);
		
		
		ManagedChannel namingServerChannel = ManagedChannelBuilder.forAddress("localhost",5000).usePlaintext().build();
		
		Class clss = new Class();
		boolean debugFlag = false;
		
		if(args.length > 0) {
			if (args[0].equals("-debug")) {
				debugFlag = true;
			}
		}
		
		final BindableService StudentImpl = (BindableService) new StudentServiceImpl(clss,debugFlag);
		final BindableService ProfessorImpl = (BindableService) new ProfessorServiceImpl(clss, debugFlag);
		final BindableService AdminImpl = (BindableService) new AdminServiceImpl(clss,debugFlag);
		final BindableService ClassServerImpl = (BindableService) new ClassServerServiceImpl(clss,debugFlag);
		
		namingServerStub = NamingServerServiceGrpc.newBlockingStub(namingServerChannel);
		
		try{
			namingServerStub.register(RegisterRequest.newBuilder().setName(name).setHost(host).setPort(port).addAllQualifiersList(qualifiersList).build());
			
		} catch (StatusRuntimeException e){
			e.printStackTrace();
		}
		
		// update channel é o canal para o servidor que vai receber os updates
		//ManagedChannel update_channel = ManagedChannelBuilder.forAddress(host,port).usePlaintext().build();
		//updateStub =  ClassServerServiceGrpc.newBlockingStub(update_channel);
		//updateStub.setUpVectorClock(SetUpVectorClockRequest.getDefaultInstance());
		boolean gossipActivity = clss.getGossipActivity();
		clss.setUpVectorClock();

		// GOSSIP 		
		Thread one = new Thread(){
			public void run(){
				new java.util.Timer().schedule (
					new java.util.TimerTask(){	
						public void run(){
							try{
								//GetGossipActivityResponse gossipResponse = updateStub.getGossipActivity(GetGossipActivityRequest.getDefaultInstance());
								//boolean gossipActivity = gossipResponse.getGossipState();
								
								if ( gossipActivity ){
									List<ServerE> gossipTo = new ArrayList<>(); // servidores para os quais vai fazer gossip
									LookupResponse response = namingServerStub.lookup(LookupRequest.newBuilder().setName(name).addAllQualifiersList(secondaryServer).build());
									List<ServerE> secondaryServers = response.getServerPortsList();
									for ( ServerE s : secondaryServers ){
										if ( s.getPort() != port )
										gossipTo.add(s);
									}
									response = namingServerStub.lookup(LookupRequest.newBuilder().setName(name).addAllQualifiersList(primaryServer).build());
									List<ServerE> primaryServer = response.getServerPortsList();
									for ( ServerE p : primaryServer ){
										if ( p.getPort() != port )
										gossipTo.add(p);
									}
									
									try{
										ManagedChannel gossipServer_channel;
										
										// estas variáveis serão utilizadas consoante os updates necessários
										Integer capacity;
										Boolean openEnrollments;
										List<Student> enrolled = new ArrayList<>();
										List<Student> discarded = new ArrayList<>();
										
										// percorremos todos os servidores
										for ( ServerE s: gossipTo){
											gossipServer_channel = ManagedChannelBuilder.forAddress(s.getHost(),s.getPort()).usePlaintext().build();
											gossipStub = ClassServerServiceGrpc.newBlockingStub(gossipServer_channel);
											List<String> vectorClockString = gossipStub.getVectorClock(GetVectorClockRequest.getDefaultInstance()).getVectorClockList();
											UpdateVectorClockResponse responsee = gossipStub.updateVectorClock(UpdateVectorClockResquest.newBuilder().setServerPort(s.getId()).addAllVectorClock(vectorClockString).build());
											List<String> updateList = responsee.getUpdatesList();
											for ( String update : updateList ){

												if ( update.equals("OPEN") || update.equals("CLOSE")){
													capacity = gossipStub.getCapacity(GetCapacityRequest.getDefaultInstance()).getCapacity();
													openEnrollments = gossipStub.getOpenEnrollments(GetOpenEnrollmentsRequest.getDefaultInstance()).getOpenEnrollments();
													gossipStub.openUpdate(OpenUpdateRequest.newBuilder().setCapacity(capacity).setOpenEnrollments(openEnrollments).build());
												}
												if ( update.equals("ENROLL") ){
													enrolled = gossipStub.getEnrolled(GetEnrolledRequest.getDefaultInstance()).getEnrolledList();
													gossipStub.enrolledUpdate(EnrolledStudentsUpdateRequest.newBuilder().addAllEnrolled(enrolled).build());
												}

												if ( update.equals("CANCEL")){
													discarded = gossipStub.getDiscarded(GetDiscardedRequest.getDefaultInstance()).getDiscardedList();
													gossipStub.discardedUpdate(DiscardedStudentsUpdateRequest.newBuilder().addAllDiscarded(discarded).build());
												}

											}
										}
									} catch (StatusRuntimeException e){
										e.printStackTrace();
									}
								}
							} catch (StatusRuntimeException e){
								System.out.println("NamingServer unavailable");
								e.printStackTrace();
							} 
						}
					}, 0, 5000);
				}
				
			};
			one.start();
			

			// Create a new server to listen on port.
			Server server = ServerBuilder.forPort(port).addService(StudentImpl).addService(ProfessorImpl).addService(AdminImpl).addService(ClassServerImpl).build();
			// Start the server.
			try {

				Runtime.getRuntime().addShutdownHook(new Thread() {
					public void run() {
						try{
							namingServerStub.delete(DeleteRequest.newBuilder().setName(name).setHost(host).setPort(port).build());
						} catch (StatusRuntimeException e){
							e.printStackTrace();
						}
					}
				});
				server.start();
			} catch (IOException e) {
				e.printStackTrace();
			}

		// Server threads are running in the background.
			System.out.println("Server started");

			// Do not exit the main thread. Wait until server is terminated.
			try {
				server.awaitTermination();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			
		}
}
