package woo;

import java.io.Serializable;

public class NormalState extends ClientState implements Serializable {

	private static final long serialVersionUID = 202012061212L;

    public NormalState(Client client) {
		super(client);
    }

    @Override
    public double getPriceP1(double currentPrice, int daysPassed) {
        return 0.9 * currentPrice;
    }

    @Override
	public double getPriceP2(double currentPrice, int daysPassed) {
        return currentPrice;
    }

    @Override
	public double getPriceP3(double currentPrice, int daysPassed) {
        return (0.05 * currentPrice) * daysPassed + currentPrice;
    }

    @Override
	public double getPriceP4(double currentPrice, int daysPassed) {
        return (0.10 * currentPrice) * daysPassed + currentPrice;
    }

    @Override
    public void updatePoints(double payedPrice, int daysPassed, double points) {
        if(daysPassed <= 0) 
            _client.setPoints(_client.getPoints() + 10 * payedPrice);
    }
    
    @Override
    public void updateState(int daysPassed, double points) {
        if(points > 25000)
            _client.setState(new EliteState(_client));
        else if(points > 2000)
            _client.setState(new SelectionState(_client));
    }

    @Override
    public String printState() {
        return "NORMAL";
    }
}

