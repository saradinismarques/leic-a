package woo.app.main;

import woo.Storefront;
import woo.exceptions.InvalidDateCoreException;
import woo.app.exceptions.InvalidDateException;
import pt.tecnico.po.ui.Command;     
import pt.tecnico.po.ui.Input;
import pt.tecnico.po.ui.DialogException;                                                            
/**
 * Advance current date.
 */
public class DoAdvanceDate extends Command<Storefront> {
  
  private Input<Integer> _daysToAdvance;

  public DoAdvanceDate(Storefront receiver) {
    super(Label.ADVANCE_DATE, receiver);
    _daysToAdvance = _form.addIntegerInput(Message.requestDaysToAdvance());
  }

  @Override
  public final void execute() throws DialogException {
    try{

		  _form.parse();
      _receiver.advanceDate(_daysToAdvance.value());

    } catch(InvalidDateCoreException e) {
      throw new InvalidDateException(_daysToAdvance.value());
    }  
  } 
}
