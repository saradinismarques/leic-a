package woo.app.suppliers;

import woo.Storefront;   
import pt.tecnico.po.ui.Command;                                    
import pt.tecnico.po.ui.DialogException; 
/**
 * Show all suppliers.
 */
public class DoShowSuppliers extends Command<Storefront> {

  public DoShowSuppliers(Storefront receiver) {
    super(Label.SHOW_ALL_SUPPLIERS, receiver);
  }

  @Override
  public void execute() throws DialogException {
    _display.popup(_receiver.getAllSuppliers());
  }
}
