package pt.ulisboa.tecnico.classes.classserver;

import io.grpc.stub.StreamObserver;
import pt.ulisboa.tecnico.classes.classserver.exceptions.*;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.professor.ProfessorClassServer.*;
import pt.ulisboa.tecnico.classes.contract.professor.ProfessorServiceGrpc;

public class ProfessorServiceImpl extends ProfessorServiceGrpc.ProfessorServiceImplBase {

	private Class clss;
	public boolean debugFlag;

	public ProfessorServiceImpl(Class clss, boolean debugFlag){
		//private Professor professor = new Professor(); Isto é para tirar?
		this.clss = clss;
		this.debugFlag = debugFlag;

	}

	@Override
	public void openEnrollments(OpenEnrollmentsRequest request, StreamObserver<OpenEnrollmentsResponse> responseObserver) {
		// StreamObserver is used to represent the gRPC stream between the server and
		// client in order to send the appropriate responses (or errors, if any occur).

		OpenEnrollmentsResponse response;
		ResponseCode code;
		try {
			clss.openEnrollments(request.getCapacity(), debugFlag);
			code = ResponseCode.OK;
			response = OpenEnrollmentsResponse.newBuilder().setCode(code).build();
		} catch (InactiveServerException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.INACTIVE_SERVER;
			response = OpenEnrollmentsResponse.newBuilder().setCode(code).build();
		} catch (EnrollmentsAlreadyOpenedException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.ENROLLMENTS_ALREADY_OPENED;
			response = OpenEnrollmentsResponse.newBuilder().setCode(code).build();
		} catch (FullClassException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.FULL_CLASS;
			response = OpenEnrollmentsResponse.newBuilder().setCode(code).build();
		}

		// Send a single response through the stream.
		responseObserver.onNext(response);
		// Notify the client that the operation has been completed.
		responseObserver.onCompleted();
	}

    @Override
	public void closeEnrollments(CloseEnrollmentsRequest request, StreamObserver<CloseEnrollmentsResponse> responseObserver) {
		// StreamObserver is used to represent the gRPC stream between the server and
		// client in order to send the appropriate responses (or errors, if any occur).
		CloseEnrollmentsResponse response;
		ResponseCode code;
		try {
			clss.closeEnrollments(debugFlag);
			code = ResponseCode.OK;
			response = CloseEnrollmentsResponse.newBuilder().setCode(code).build();
		} catch (InactiveServerException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.INACTIVE_SERVER;
			response = CloseEnrollmentsResponse.newBuilder().setCode(code).build();
		} catch (EnrollmentsAlreadyClosedException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.ENROLLMENTS_ALREADY_CLOSED;
			response = CloseEnrollmentsResponse.newBuilder().setCode(code).build();
		}
		// Send a single response through the stream.
		responseObserver.onNext(response);
		// Notify the client that the operation has been completed.
		responseObserver.onCompleted();
	}

	@Override
    public void listClass(ListClassRequest request, StreamObserver<ListClassResponse> responseObserver) {
		// StreamObserver is used to represent the gRPC stream between the server and
		// client in order to send the appropriate responses (or errors, if any occur).

		ListClassResponse response;
		ClassState classState;
		ResponseCode code;
		try {
			classState = clss.listClass(debugFlag);
			code = ResponseCode.OK;
			response = ListClassResponse.newBuilder().setCode(code).setClassState(classState).build();
		} catch (InactiveServerException e) {
			System.out.println("Caught exception with description: " + e);
			code = ResponseCode.INACTIVE_SERVER;
			response = ListClassResponse.newBuilder().setCode(code).build();
		}

		// Send a single response through the stream.
		responseObserver.onNext(response);
		// Notify the client that the operation has been completed.
		responseObserver.onCompleted();
	}

    @Override
    	public void cancelEnrollment(CancelEnrollmentRequest request, StreamObserver<CancelEnrollmentResponse> responseObserver) {
    		// StreamObserver is used to represent the gRPC stream between the server and
    		// client in order to send the appropriate responses (or errors, if any occur).

			CancelEnrollmentResponse response;
			ResponseCode code;
			try {
				clss.cancelEnrollment(request.getStudentId(), debugFlag);
				code = ResponseCode.OK;
				response = CancelEnrollmentResponse.newBuilder().setCode(code).build();
			} catch (InactiveServerException e) {
				System.out.println("Caught exception with description: " + e);
				code = ResponseCode.INACTIVE_SERVER;
				response = CancelEnrollmentResponse.newBuilder().setCode(code).build();
			} catch (NonExistingStudentException e) {
				System.out.println("Caught exception with description: " + e);
				code = ResponseCode.NON_EXISTING_STUDENT;
				response = CancelEnrollmentResponse.newBuilder().setCode(code).build();
			}
    		// Send a single response through the stream.
    		responseObserver.onNext(response);
    		// Notify the client that the operation has been completed.
    		responseObserver.onCompleted();
    	}		

}