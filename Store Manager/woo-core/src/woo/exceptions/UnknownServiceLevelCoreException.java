package woo.exceptions;

/** Exception for unknown service levels. */
public class UnknownServiceLevelCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011072008L;

  /** Unknown level. */
  private String _level;

  /** @param level Unknown level to report. */
  public UnknownServiceLevelCoreException(String level) {
    _level = level;
  }
}
