package pt.ulisboa.tecnico.classes.classserver;

import java.util.ArrayList;
import java.util.List;

import io.grpc.stub.StreamObserver;
import pt.ulisboa.tecnico.classes.classserver.exceptions.*;
import pt.ulisboa.tecnico.classes.contract.ClassesDefinitions.*;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerClassServer.*;
import pt.ulisboa.tecnico.classes.contract.classserver.ClassServerServiceGrpc;

public class ClassServerServiceImpl  extends ClassServerServiceGrpc.ClassServerServiceImplBase {

    private Class clss;
    public boolean debugFlag;

    public ClassServerServiceImpl(Class clss, boolean debugFlag){
		this.clss = clss;
        this.debugFlag = debugFlag;
    }

    @Override
    public void propagateState(PropagateStateRequest request, StreamObserver<PropagateStateResponse> responseObserver){

        PropagateStateResponse response;
        ResponseCode code;
        
        try {
            clss.propagateState(request.getClassState(),debugFlag);
            code = ResponseCode.OK;
            response = PropagateStateResponse.newBuilder().setCode(code).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.INACTIVE_SERVER;
            response = PropagateStateResponse.newBuilder().setCode(code).build();
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();

    }

    @Override
    public void updateVectorClock(UpdateVectorClockResquest request, StreamObserver<UpdateVectorClockResponse> responseObserver){

        UpdateVectorClockResponse response;
        ResponseCode code;

        List<String> updateList = new ArrayList<>();
        
        try {
            updateList = clss.updateVectorClock(request.getServerPort(),request.getVectorClockList());
            code = ResponseCode.OK;
            response = UpdateVectorClockResponse.newBuilder().addAllUpdates(updateList).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.INACTIVE_SERVER;
            response = UpdateVectorClockResponse.newBuilder().addAllUpdates(updateList).build();
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();

    }

    

    @Override
    public void openUpdate(OpenUpdateRequest request, StreamObserver<OpenUpdateResponse> responseObserver ){
        
        OpenUpdateResponse response;
        ResponseCode code;

        try {
            clss.openUpdate(request.getCapacity(),request.getOpenEnrollments());
            code = ResponseCode.OK;
            response = OpenUpdateResponse.newBuilder().setCode(code).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.INACTIVE_SERVER;
            response = OpenUpdateResponse.newBuilder().setCode(code).build();
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void enrolledUpdate(EnrolledStudentsUpdateRequest request, StreamObserver<EnrolledStudentsUpdateResponse> responseObserver ){
        
        EnrolledStudentsUpdateResponse response;
        ResponseCode code;

        try {
            clss.enrolledUpdate(request.getEnrolledList());
            code = ResponseCode.OK;
            response = EnrolledStudentsUpdateResponse.newBuilder().setCode(code).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
            code = ResponseCode.INACTIVE_SERVER;
            response = EnrolledStudentsUpdateResponse.newBuilder().setCode(code).build();
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void setUpVectorClock(SetUpVectorClockRequest request, StreamObserver<SetUpVectorClockResponse> responseObserver) {
        SetUpVectorClockResponse response = SetUpVectorClockResponse.getDefaultInstance();

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void getCapacity(GetCapacityRequest request, StreamObserver<GetCapacityResponse> responseObserver ){
        
        GetCapacityResponse response = GetCapacityResponse.getDefaultInstance();
        
        try {
            int capacity = clss.getCapacity();
            response = GetCapacityResponse.newBuilder().setCapacity(capacity).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }


    @Override
    public void getOpenEnrollments(GetOpenEnrollmentsRequest request, StreamObserver<GetOpenEnrollmentsResponse> responseObserver ){
        
        GetOpenEnrollmentsResponse response = GetOpenEnrollmentsResponse.getDefaultInstance();
        
        try {
            boolean openEnrollments = clss.getOpenEnrollments();
            response = GetOpenEnrollmentsResponse.newBuilder().setOpenEnrollments(openEnrollments).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }


    @Override
    public void getEnrolled(GetEnrolledRequest request, StreamObserver<GetEnrolledResponse> responseObserver ){
        
        GetEnrolledResponse response = GetEnrolledResponse.getDefaultInstance();
        
        try {
            ArrayList<Student> enrolledStudents = clss.getEnrolledStudents();
            response = GetEnrolledResponse.newBuilder().addAllEnrolled(enrolledStudents).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void getDiscarded(GetDiscardedRequest request, StreamObserver<GetDiscardedResponse> responseObserver ){
        
        GetDiscardedResponse response = GetDiscardedResponse.getDefaultInstance();
        
        try {
            ArrayList<Student> discardedStudents = clss.getDiscardedStudent();
            response = GetDiscardedResponse.newBuilder().addAllDiscarded(discardedStudents).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
        }

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void getVectorClock(GetVectorClockRequest request, StreamObserver<GetVectorClockResponse> responseObserver ){
        
        GetVectorClockResponse response = GetVectorClockResponse.getDefaultInstance();
        
        List<String> vectorClock = clss.getVectorClock();
        response = GetVectorClockResponse.newBuilder().addAllVectorClock(vectorClock).build();

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }

    @Override
    public void getGossipActivity(GetGossipActivityRequest request, StreamObserver<GetGossipActivityResponse> responseObserver ){
        
        GetGossipActivityResponse response = GetGossipActivityResponse.getDefaultInstance();
        
        /*try {
            boolean gossipActivity = clss.getGossipActivity();
            response = GetGossipActivityResponse.newBuilder().setGossipState(gossipActivity).build();
        } catch (InactiveServerException e) {
            System.out.println("Caught exception with description: " + e);
        }*/

        responseObserver.onNext(response);
        responseObserver.onCompleted();
    }
    


    
}
