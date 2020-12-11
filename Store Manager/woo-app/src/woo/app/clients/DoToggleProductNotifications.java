package woo.app.clients;

import pt.tecnico.po.ui.Command;      
import pt.tecnico.po.ui.DialogException;  
import pt.tecnico.po.ui.Input;              
import woo.Storefront;             
import woo.exceptions.UnknownClientKeyCoreException;
import woo.exceptions.UnknownProductKeyCoreException;
import woo.app.exceptions.UnknownClientKeyException;
import woo.app.exceptions.UnknownProductKeyException;
/**
 * Toggle product-related notifications.
 */
public class DoToggleProductNotifications extends Command<Storefront> {

  private Input<String> _clientKey, _productKey;

  public DoToggleProductNotifications(Storefront storefront) {
    super(Label.TOGGLE_PRODUCT_NOTIFICATIONS, storefront);
    _clientKey = _form.addStringInput(Message.requestClientKey());
    _productKey = _form.addStringInput(Message.requestProductKey());
  }

  @Override
  public void execute() throws DialogException {
    boolean isActive;

    try {

      _form.parse();
      isActive = _receiver.toggleNotifications(_clientKey.value(), _productKey.value());

      if(isActive)
        _display.popup(Message.notificationsOn(_clientKey.value(), _productKey.value()));

      else
        _display.popup(Message.notificationsOff(_clientKey.value(), _productKey.value()));
        
    } catch(UnknownClientKeyCoreException e){
      throw new UnknownClientKeyException(_clientKey.value());
    } catch(UnknownProductKeyCoreException e){
      throw new UnknownProductKeyException(_productKey.value());
    }
  }

}

