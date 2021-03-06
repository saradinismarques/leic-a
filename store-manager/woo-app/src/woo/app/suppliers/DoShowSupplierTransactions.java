package woo.app.suppliers;

import pt.tecnico.po.ui.Command;            
import pt.tecnico.po.ui.DialogException;     
import pt.tecnico.po.ui.Input;              
import woo.Storefront;                     
import woo.exceptions.UnknownSupplierKeyCoreException;      
import woo.app.exceptions.UnknownSupplierKeyException; 
/**
 * Show all transactions for specific supplier.
 */
public class DoShowSupplierTransactions extends Command<Storefront> {

  private Input<String> _key;

  public DoShowSupplierTransactions(Storefront receiver) {
    super(Label.SHOW_SUPPLIER_TRANSACTIONS, receiver);
    _key = _form.addStringInput(Message.requestSupplierKey());
  }

  @Override
  public void execute() throws DialogException {
    try {

      _form.parse();
      _display.popup(_receiver.getSupplierTransactions(_key.value()));
      
    } catch(UnknownSupplierKeyCoreException e){
      throw new UnknownSupplierKeyException(_key.value());
    }
  }

}
