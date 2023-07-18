package pt.ulisboa.tecnico.classes.classserver.exceptions;

public class EnrollmentsAlreadyOpenedException extends Exception {
    public EnrollmentsAlreadyOpenedException(String errorMessage) {
        super(errorMessage);
    }
}