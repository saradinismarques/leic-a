package woo.app.products;

import woo.Storefront;                                                                                 
import pt.tecnico.po.ui.Command; 
import pt.tecnico.po.ui.DialogException;  
import pt.tecnico.po.ui.Display; 
/**
 * Show all products.
 */
public class DoShowAllProducts extends Command<Storefront> {

  public DoShowAllProducts(Storefront receiver) {
    super(Label.SHOW_ALL_PRODUCTS, receiver);
  }

  @Override
  public final void execute() throws DialogException {		  
    _display.popup(_receiver.getAllProducts());
  }

}
