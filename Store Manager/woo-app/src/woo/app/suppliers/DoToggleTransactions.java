package woo.app.suppliers;

import pt.tecnico.po.ui.Command;   
import pt.tecnico.po.ui.DialogException;    
import pt.tecnico.po.ui.Input;               
import woo.Storefront;                      
import woo.exceptions.UnknownSupplierKeyCoreException;      
import woo.app.exceptions.UnknownSupplierKeyException; 
/**
 * Enable/disable supplier transactions.
 */
public class DoToggleTransactions extends Command<Storefront> {

  private Input<String> _key;

  public DoToggleTransactions(Storefront receiver) {
    super(Label.TOGGLE_TRANSACTIONS, receiver);
    _key = _form.addStringInput(Message.requestSupplierKey());
  }

  @Override
  public void execute() throws DialogException {
    boolean isActive;

    try {

      _form.parse();
      isActive = _receiver.toggleTransactions(_key.value());

      if(isActive)
        _display.popup(Message.transactionsOn(_key.value()));

      else
        _display.popup(Message.transactionsOff(_key.value()));

    } catch(UnknownSupplierKeyCoreException e){
      throw new UnknownSupplierKeyException(_key.value());
    }
  }

}
