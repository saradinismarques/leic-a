package woo.app.main;

import woo.Storefront;
import pt.tecnico.po.ui.Command;                                                                     
import pt.tecnico.po.ui.Display;
import pt.tecnico.po.ui.DialogException;
/**
 * Show current date.
 */
public class DoDisplayDate extends Command<Storefront> {

  public DoDisplayDate(Storefront receiver) {
    super(Label.SHOW_DATE, receiver);
  }

  @Override
  public final void execute() throws DialogException {
    _display.popup(Message.currentDate(_receiver.getDate()));
  }
}
