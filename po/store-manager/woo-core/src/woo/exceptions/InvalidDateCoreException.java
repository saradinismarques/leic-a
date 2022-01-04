package woo.exceptions;

/** Exception for date-related problems. */
public class InvalidDateCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011071808L;

  /** Bad date. */
  private int _date;

  /** @param date bad date to report. */
  public InvalidDateCoreException(int date) {
    _date = date;
  }
}
