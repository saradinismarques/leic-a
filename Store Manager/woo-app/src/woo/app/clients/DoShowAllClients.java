package woo.app.clients;
 
import woo.Storefront;
import pt.tecnico.po.ui.Command;                  
import pt.tecnico.po.ui.DialogException;
import pt.tecnico.po.ui.Display;
/**
 * Show all clients.
 */
public class DoShowAllClients extends Command<Storefront> {

  public DoShowAllClients(Storefront storefront) {
    super(Label.SHOW_ALL_CLIENTS, storefront);
  }

  @Override
  public void execute() throws DialogException {
    _display.popup(_receiver.getAllClients());
  }
}
