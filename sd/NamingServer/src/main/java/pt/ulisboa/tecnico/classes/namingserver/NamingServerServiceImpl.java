package pt.ulisboa.tecnico.classes.namingserver;

import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerClassServer.PropagateStateRequest;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerClassServer.PropagateStateResponse;
import pt.ulisboa.tecnico.classes.contract.naming.NamingServerServiceGrpc;
import io.grpc.stub.StreamObserver;

// porque nao temos imports do proto?
import pt.ulisboa.tecnico.classes.contract.naming.ClassServerNamingServer.*;


import java.util.List;

public class NamingServerServiceImpl extends NamingServerServiceGrpc.NamingServerServiceImplBase {
    
    public NamingServices namingServices = new NamingServices();

    public NamingServerServiceImpl(NamingServices namingServices){
        this.namingServices = namingServices;
    }

    @Override
	public void register(RegisterRequest request, StreamObserver<RegisterResponse> responseObserver){
        RegisterResponse response = RegisterResponse.getDefaultInstance();

        namingServices.register(request.getName(), request.getHost(), request.getPort() , request.getQualifiersListList());

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
	public void lookup(LookupRequest request, StreamObserver<LookupResponse> responseObserver){
        
        LookupResponse response;

        List<ServerE> serverEntries = namingServices.lookup(request.getName(), request.getQualifiersListList());
        
        response = LookupResponse.newBuilder().addAllServerPorts(serverEntries).build();
        
        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
	public void delete(DeleteRequest request, StreamObserver<DeleteResponse> responseObserver){
        
        DeleteResponse response = DeleteResponse.getDefaultInstance();

        namingServices.delete(request.getName(), request.getHost(), request.getPort());

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    
}
