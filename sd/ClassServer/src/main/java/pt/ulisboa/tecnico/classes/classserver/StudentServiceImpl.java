package pt.ulisboa.tecnico.classes.classserver;

import io.grpc.stub.StreamObserver;
import pt.ulisboa.tecnico.classes.classserver.exceptions.*;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.student.StudentClassServer.*;
import pt.ulisboa.tecnico.classes.contract.student.StudentServiceGrpc;

public class StudentServiceImpl extends StudentServiceGrpc.StudentServiceImplBase {
	//private Student student = new Student();

    private Class clss;
    public boolean debugFlag;

    public StudentServiceImpl(Class clss, boolean debugFlag){
		this.clss = clss;
        this.debugFlag = debugFlag;
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
    public void enroll(EnrollRequest request, StreamObserver<EnrollResponse> responseObserver) {
        // StreamObserver is used to represent the gRPC stream between the server and
        // client in order to send the appropriate responses (or errors, if any occur).
        EnrollResponse response;
        ResponseCode code;

        try {
            clss.enroll(request.getStudent(), debugFlag);
            code = ResponseCode.OK;
            response = EnrollResponse.newBuilder().setCode(code).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.INACTIVE_SERVER;
            response = EnrollResponse.newBuilder().setCode(code).build();
        } catch (EnrollmentsAlreadyClosedException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.ENROLLMENTS_ALREADY_CLOSED;
            response = EnrollResponse.newBuilder().setCode(code).build();
        } catch (StudentAlreadyEnrolledException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.STUDENT_ALREADY_ENROLLED;
            response = EnrollResponse.newBuilder().setCode(code).build();
        } catch (FullClassException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.FULL_CLASS;
            response = EnrollResponse.newBuilder().setCode(code).build();
        }
        // Send a single response through the stream.
        responseObserver.onNext(response);
        // Notify the client that the operation has been completed.
        responseObserver.onCompleted();
    }

}