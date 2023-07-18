package pt.ulisboa.tecnico.classes.classserver;

import io.grpc.stub.StreamObserver;

import pt.ulisboa.tecnico.classes.classserver.exceptions.InactiveServerException;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.admin.AdminClassServer.*;
import pt.ulisboa.tecnico.classes.contract.admin.AdminServiceGrpc;

public class AdminServiceImpl extends AdminServiceGrpc.AdminServiceImplBase {

	public Class clss;
	public boolean debugFlag;

    public AdminServiceImpl(Class clss, boolean debugFlag){

		this.clss = clss;
		this.debugFlag = debugFlag;
	}

	@Override
	public void activate(ActivateRequest request, StreamObserver<ActivateResponse> responseObserver) {
		// StreamObserver is used to represent the gRPC stream between the server and
		// client in order to send the appropriate responses (or errors, if any occur).
        ActivateResponse response;
		ResponseCode code;

		clss.activate(debugFlag);
		code = ResponseCode.OK;
		response = ActivateResponse.newBuilder().setCode(code).build();
	
		// Send a single response through the stream.
		responseObserver.onNext(response);
		// Notify the client that the operation has been completed.
		responseObserver.onCompleted();
	}

	@Override
	public void deactivate(DeactivateRequest request, StreamObserver<DeactivateResponse> responseObserver) {
		// StreamObserver is used to represent the gRPC stream between the server and
		// client in order to send the appropriate responses (or errors, if any occur).
        DeactivateResponse response;
		ResponseCode code;

		System.out.println("yooo");
		clss.deactivate(debugFlag);
		System.out.println("yooo 1");

		code = ResponseCode.OK;
		response = DeactivateResponse.newBuilder().setCode(code).build();

		// Send a single response through the stream.
		responseObserver.onNext(response);
		// Notify the client that the operation has been completed.
		responseObserver.onCompleted();
	}

    @Override
	public void dump(DumpRequest request, StreamObserver<DumpResponse> responseObserver) {
		// StreamObserver is used to represent the gRPC stream between the server and
		// client in order to send the appropriate responses (or errors, if any occur).
		DumpResponse response;
		ClassState classState;
		ResponseCode code;
		try {
			classState = clss.dump(debugFlag);
			code = ResponseCode.OK;
			response = DumpResponse.newBuilder().setCode(code).setClassState(classState).build();
		} catch (InactiveServerException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.INACTIVE_SERVER;
			response = DumpResponse.newBuilder().setCode(code).build();
		}
		// Send a single response through the stream.
		responseObserver.onNext(response);
		// Notify the client that the operation has been completed.
		responseObserver.onCompleted();
	}

	@Override
	public void deactivateGossip(DeactivateGossipRequest request, StreamObserver<DeactivateGossipResponse> responseObserver) {
		
		DeactivateGossipResponse response;
		ResponseCode code;

		try {
			clss.deactivateGossip(debugFlag);
			code = ResponseCode.OK;
			response = DeactivateGossipResponse.newBuilder().setCode(code).build();
		} catch (InactiveServerException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.INACTIVE_SERVER;
			response = DeactivateGossipResponse.newBuilder().setCode(code).build();
		}

	}


	@Override
	public void activateGossip(ActivateGossipRequest request, StreamObserver<ActivateGossipResponse> responseObserver) {
		
		ActivateGossipResponse response;
		ResponseCode code;

		try {
			clss.activateGossip(debugFlag);
			code = ResponseCode.OK;
			response = ActivateGossipResponse.newBuilder().setCode(code).build();
		} catch (InactiveServerException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.INACTIVE_SERVER;
			response = ActivateGossipResponse.newBuilder().setCode(code).build();
		}

	}

	@Override
	public void gossip(GossipRequest request, StreamObserver<GossipResponse> responseObserver) {
		
		GossipResponse response;
		ResponseCode code;

		try {
			clss.gossip(debugFlag);
			code = ResponseCode.OK;
			response = GossipResponse.newBuilder().setCode(code).build();
		} catch (InactiveServerException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.INACTIVE_SERVER;
			response = GossipResponse.newBuilder().setCode(code).build();
		}

	}
}