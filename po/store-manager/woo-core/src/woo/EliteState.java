package woo;

import java.io.Serializable;

public class EliteState extends ClientState implements Serializable {

	private static final long serialVersionUID = 202012061211L;

    public EliteState(Client client) {
		super(client);
    }

    @Override
    public double getPriceP1(double currentPrice, int daysPassed) {
        return 0.9 * currentPrice;
    }

    @Override
	public double getPriceP2(double currentPrice, int daysPassed) {
        return 0.9 * currentPrice;
    }

    @Override
	public double getPriceP3(double currentPrice, int daysPassed) {
        return 0.95 * currentPrice;  
    }

    @Override
	public double getPriceP4(double currentPrice, int daysPassed) {
        return currentPrice;
    }

    @Override
    public void updatePoints(double payedPrice, int daysPassed, double points) {
        if(daysPassed <= 0) 
            _client.setPoints(_client.getPoints() + 10 * payedPrice);
        else if(daysPassed > 15)
            _client.setPoints(_client.getPoints() * 0.25);
    }

    @Override
    public void updateState(int daysPassed, double points) {
        if(daysPassed > 15)
            _client.setState(new SelectionState(_client));
    }

    @Override
    public String printState() {
        return "ELITE";
    }    
}
